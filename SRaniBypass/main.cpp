#include <cassert>
#include <windows.h>
#include <iostream>

#include "tobii.h"

void url_receiver( char const* url, void* user_data )
{
    char* buffer = (char*)user_data;
    if( *buffer != '\0' ) return; // only keep first value

    if( strlen( url ) < 256 )
        strcpy( buffer, url );
}

void wearable_callback( tobii_wearable_data_t const* data, void* user_data )
{
    std::cout << "Got Data" << std::endl;
}

typedef void(*tobii_enumerate_callback)(char const* url, void* user_data);
typedef void(*tobii_wearable_data_callback)(tobii_wearable_data_t const* data, void* user_data);

typedef tobii_error_t (*tobii_api_create)(void*, void*, void*);
typedef tobii_error_t (*tobii_get_api_version)(tobii_version_t*);
typedef tobii_error_t (*tobii_enumerate_local_device_urls)(void*, tobii_enumerate_callback, char*);
typedef tobii_error_t (*tobii_create_device)(void* api, char const* url, void* device);
typedef tobii_error_t (*tobii_get_device_info)(void*, tobii_device_info_t*);
typedef tobii_error_t (*tobii_wearable_data_subscribe)(void*, tobii_wearable_data_callback, INT64*);
typedef tobii_error_t (*tobii_wait_for_callbacks)(int, void* const*);
typedef tobii_error_t (*tobii_device_process_callbacks)(void*);
typedef int (*enable_eye_feature)(INT64, INT64);
typedef INT64 (*FindDevice)(char*, char*);

static void* api;
static void* device;
static tobii_device_info_t info;

FindDevice find_device_sig;
enable_eye_feature enable_eye_feature_sig;
tobii_api_create create_api_sig;
tobii_get_api_version version_func_sig;
tobii_enumerate_local_device_urls enumerate_func_sig;
tobii_create_device create_device_sig;
tobii_get_device_info device_info_sig;
tobii_wearable_data_subscribe wearable_data_sig;
tobii_wait_for_callbacks tobii_wait_for_callbacks_sig;
tobii_device_process_callbacks tobii_device_process_callbacks_sig;

int tobii_init()
{
    std::cout << "Connection established." << std::endl;
    tobii_error_t init_error = create_api_sig( &api, NULL, NULL);
    if (init_error != TOBII_ERROR_NO_ERROR)
    {
        std::cout << "Failed to initialize tobii stream api" << std::endl;
        return EXIT_FAILURE;
    }
    else
        std::cout << "Tobii stream engine initialized" << std::endl;
    
    tobii_version_t version;
    const int versionError = version_func_sig(&version);
    if (versionError != TOBII_ERROR_NO_ERROR)
    {
        std::cout << "Failed to get tobii stream engine version" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout <<  "Current API version: " << version.major << "." << version.minor << "." << version.revision << std::endl;
    
    char url[ 256 ] = { 0 };
    enumerate_func_sig(api, url_receiver, url);
            
    tobii_error_t error = create_device_sig(api, url, &device);
            
    tobii_error_t subscribe_error = wearable_data_sig(device, wearable_callback, 0);
            
    while( true )
    {
        tobii_error_t wait_error = tobii_wait_for_callbacks_sig( 1, &device );
        assert( wait_error == TOBII_ERROR_NO_ERROR || wait_error == TOBII_ERROR_TIMED_OUT );

        tobii_error_t process_error = tobii_device_process_callbacks_sig( device );
        assert( process_error == TOBII_ERROR_NO_ERROR );
    }
}

int main()
{
    std::string sranipalBaseDir = "C:\\Program Files\\VIVE\\SRanipal\\";
    std::string vpeBaseDir = sranipalBaseDir + "eye_camera_device\\vive_pro_eye\\";
    
    HINSTANCE tobiiStreamEngine = LoadLibrary((vpeBaseDir + "tobii_stream_engine.dll").c_str());

    if (!tobiiStreamEngine)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }
    
    HINSTANCE usbDevConnect = LoadLibrary((vpeBaseDir + "USBDevConnectDLL.dll").c_str());

    if (!usbDevConnect)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }
    
    HINSTANCE libEChip = LoadLibrary((vpeBaseDir + "libEChipConnection.dll").c_str());

    if (!libEChip)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    
    HINSTANCE opencv = LoadLibrary((sranipalBaseDir + "opencv_world310.dll").c_str());

    if (!opencv)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }
    
    HINSTANCE eyeDeviceConfig = LoadLibrary((vpeBaseDir + "EyeDeviceConfiguration.dll").c_str());

    if (!eyeDeviceConfig)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    // resolve function address here
    find_device_sig = (FindDevice)GetProcAddress(eyeDeviceConfig, "FindDevice");
    enable_eye_feature_sig = (enable_eye_feature)GetProcAddress(libEChip, "EnableEyeFeature");
    create_api_sig = (tobii_api_create)GetProcAddress(tobiiStreamEngine, "tobii_api_create");
    version_func_sig = (tobii_get_api_version)GetProcAddress(tobiiStreamEngine, "tobii_get_api_version");
    enumerate_func_sig = (tobii_enumerate_local_device_urls)GetProcAddress(tobiiStreamEngine, "tobii_enumerate_local_device_urls");
    create_device_sig = (tobii_create_device)GetProcAddress(tobiiStreamEngine, "tobii_device_create");
    device_info_sig = (tobii_get_device_info)GetProcAddress(tobiiStreamEngine, "tobii_get_device_info");
    wearable_data_sig = (tobii_wearable_data_subscribe)GetProcAddress(tobiiStreamEngine, "tobii_wearable_data_subscribe");
    tobii_wait_for_callbacks_sig = (tobii_wait_for_callbacks)GetProcAddress(tobiiStreamEngine, "tobii_wait_for_callbacks");
    tobii_device_process_callbacks_sig = (tobii_device_process_callbacks)GetProcAddress(tobiiStreamEngine, "tobii_device_process_callbacks");

    
    if (!create_api_sig || !version_func_sig || !enumerate_func_sig || !create_device_sig || !device_info_sig || !enable_eye_feature_sig)
    {
        std::cout << "Failed to locate one or more functions" << std::endl;
        return EXIT_FAILURE;
    }

    int resp = enable_eye_feature_sig(0, 1);    // Initialize lib-e chip. Gives us a small window to establish connection.
    if (!resp)
    {
        char v9[2];
        char v13;
        for (int i=0; i<5; ++i)
        {
            find_device_sig(v9, &v13);
            if (v9[0])
                break;
            Sleep(2000);
        }
        if (v9[0])
            return tobii_init();
    }
    
    return 0;
}