#include "SpringAudioApi_NativeWindows.hpp"

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <combaseapi.h>
#include <Dshow.h>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

// Size of an audio frame = channelsCount * sampleSize
//                        = nChannels * wBitsPerSample

uint32_t channelsCount;
uint32_t bitsPerSample;
uint32_t samplesCountPerSecond;
int gCounter = 0;

int frequence = 440;
int period = samplesCountPerSecond/frequence;

HRESULT setFormat(WAVEFORMATEX* format)
{
    WAVEFORMATEXTENSIBLE* f2 = (WAVEFORMATEXTENSIBLE*)format;
    channelsCount = format->nChannels;
    bitsPerSample = format->wBitsPerSample;
    samplesCountPerSecond = format->nSamplesPerSec;
    period = samplesCountPerSecond/frequence;
    return TRUE;
}

HRESULT loadData(UINT32 framesCount, BYTE* data, DWORD* flags)
{
    const uint32_t frameSize = channelsCount * bitsPerSample;
    float* ptr = (float*)data;
    int mid = period / 2;
    const float volume = 1.0f;

    uint16_t squareWaveCounter = 0;

    for (UINT32 sample = 0; sample < framesCount; sample++)
    {
        //ptrLeft = data + (sample * bitsPerSample * channelsCount) / 8;
        //ptrRight = ptrLeft + bitsPerSample / 8;
        if(!squareWaveCounter)
        {
            squareWaveCounter = period;
        }

        BYTE sampleValue = (squareWaveCounter > mid) ? volume : 0.0f;
        *ptr++ = sampleValue;
        *ptr++ = sampleValue;
        --squareWaveCounter;
        //ptr+=frameSize/8;
    }
    if(gCounter < 15)
        (*flags) = 0;
    else
        (*flags) = AUDCLNT_BUFFERFLAGS_SILENT;
    gCounter ++;
    return TRUE;
}


namespace spring::audio
{
	SpringAudioApi_NativeWindows::SpringAudioApi_NativeWindows()
	{
		const spNativeLib mmdeviceApi = core::loadLibrary("Mmdevapi.dll");
		if (!mmdeviceApi)
			SPRING_ERROR("Failed to load Mmdevapi.dll");

		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
        const IID IID_IAudioClient = __uuidof(IAudioClient);
        const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

		IMMDeviceEnumerator* pEnumerator = NULL;
        HRESULT hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&pEnumerator));
		spdlog::trace("{0:x}", hr);



		//HRESULT hr;
        REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
        REFERENCE_TIME hnsActualDuration;
        //IMMDeviceEnumerator *pEnumerator = NULL;
        IMMDevice *pDevice = NULL;
        IAudioClient2 *pAudioClient = NULL;
        IAudioRenderClient *pRenderClient = NULL;
        WAVEFORMATEX *pwfx = NULL;
        UINT32 bufferFrameCount;
        UINT32 numFramesAvailable;
        UINT32 numFramesPadding;
        BYTE *pData;
        DWORD flags = 0;

        //hr = CoCreateInstance(
        //       CLSID_MMDeviceEnumerator, NULL,
        //       CLSCTX_ALL, IID_IMMDeviceEnumerator,
        //       (void**)&pEnumerator);

        hr = pEnumerator->GetDefaultAudioEndpoint(
                            eRender, eConsole, &pDevice);

        pEnumerator->Release();

        hr = pDevice->Activate(
                        __uuidof(IAudioClient2), CLSCTX_ALL,
                        NULL, (void**)&pAudioClient);

        pDevice->Release();

        hr = pAudioClient->GetMixFormat(&pwfx);

        hr = pAudioClient->Initialize(
                             AUDCLNT_SHAREMODE_SHARED,
                             0,
                             hnsRequestedDuration,
                             0,
                             pwfx,
                             NULL);

        // Tell the audio source which format to use.
        hr = setFormat(pwfx);
        spdlog::trace("Format : {} channels, {} samples per second, {} bits per sample", pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->wBitsPerSample);

        // Get the actual size of the allocated buffer.
        hr = pAudioClient->GetBufferSize(&bufferFrameCount);

        hr = pAudioClient->GetService(
                             IID_IAudioRenderClient,
                             (void**)&pRenderClient);

        // Grab the entire buffer for the initial fill operation.
        hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);

        // Load the initial data into the shared buffer.
        hr = loadData(bufferFrameCount, pData, &flags);

        hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);

        // Calculate the actual duration of the allocated buffer.
        hnsActualDuration = (double)REFTIMES_PER_SEC *
                            bufferFrameCount / pwfx->nSamplesPerSec;

        hr = pAudioClient->Start();  // Start playing.

        spdlog::info("pre-loop");

        // Each loop fills about half of the shared buffer.
        while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
        {
            // Sleep for half the buffer duration.
            Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

            // See how much buffer space is available.
            hr = pAudioClient->GetCurrentPadding(&numFramesPadding);

            numFramesAvailable = bufferFrameCount - numFramesPadding;

            // Grab all the available space in the shared buffer.
            hr = pRenderClient->GetBuffer(numFramesAvailable, (BYTE**)&pData);

            // Get next 1/2-second of data from the audio source.
            spdlog::info("Feeding!");
            hr = loadData(numFramesAvailable, pData, &flags);
            //flags |= AUDCLNT_BUFFERFLAGS_SILENT;

            hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        }
        spdlog::info("Pre-exit");

        // Wait for last data in buffer to play before stopping.
        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

        hr = pAudioClient->Stop();  // Stop playing.


		spdlog::info("Successfully initialized native windows audio!");
	}
}