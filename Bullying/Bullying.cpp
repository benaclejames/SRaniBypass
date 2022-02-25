#include <cassert>
#include <fstream>
#include <windows.h>
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS

// Borrowing from https://stackoverflow.com/a/27296/832621
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

struct tobii_version_t
{
    int major = 0;
    int minor = 0;
    int revision = 0;
};

typedef enum tobii_error_t
{
    TOBII_ERROR_NO_ERROR,
    TOBII_ERROR_INTERNAL,
    TOBII_ERROR_INSUFFICIENT_LICENSE,
    TOBII_ERROR_NOT_SUPPORTED,
    TOBII_ERROR_NOT_AVAILABLE,
    TOBII_ERROR_CONNECTION_FAILED,
    TOBII_ERROR_TIMED_OUT,
    TOBII_ERROR_ALLOCATION_FAILED,
    TOBII_ERROR_INVALID_PARAMETER,
    TOBII_ERROR_CALIBRATION_ALREADY_STARTED,
    TOBII_ERROR_CALIBRATION_NOT_STARTED,
    TOBII_ERROR_ALREADY_SUBSCRIBED,
    TOBII_ERROR_NOT_SUBSCRIBED,
    TOBII_ERROR_OPERATION_FAILED,
    TOBII_ERROR_CONFLICTING_API_INSTANCES,
    TOBII_ERROR_CALIBRATION_BUSY,
    TOBII_ERROR_CALLBACK_IN_PROGRESS,
    TOBII_ERROR_TOO_MANY_SUBSCRIBERS,
    TOBII_ERROR_CONNECTION_FAILED_DRIVER,
} tobii_error_t;

enum tobii_field_of_use_t
{
    TOBII_FIELD_OF_USE_INTERACTIVE = 0,
    TOBII_FIELD_OF_USE_HMD = 1,
    TOBII_FIELD_OF_USE_EYE_TRACKING = 2,
    TOBII_FIELD_OF_USE_ALL = 3
};

void url_receiver( char const* url, void* user_data )
{
    char* buffer = (char*)user_data;
    if( *buffer != '\0' ) return; // only keep first value

    if( strlen( url ) < 256 )
        strcpy( buffer, url );
}

typedef struct tobii_device_info_t
{
    char serial_number[256];
    char model[256];
    char generation[256];
    char firmware_version[256];
    char integration_id[128];
    char hw_calibration_version[128];
    char hw_calibration_date[128];
    char lot_id[128];
    char integration_type[256];
    char runtime_build_version[256];
} tobii_device_info_t;


typedef enum tobii_validity_t
{
    TOBII_VALIDITY_INVALID,
    TOBII_VALIDITY_VALID
} tobii_validity_t;

typedef enum tobii_wearable_tracking_improvement_t
{
    TOBII_WEARABLE_TRACKING_IMPROVEMENT_USER_POSITION_HMD,
    TOBII_WEARABLE_TRACKING_IMPROVEMENT_CALIBRATION_CONTAINS_POOR_DATA,
    TOBII_WEARABLE_TRACKING_IMPROVEMENT_CALIBRATION_DIFFERENT_BRIGHTNESS,
    TOBII_WEARABLE_TRACKING_IMPROVEMENT_IMAGE_QUALITY,
    TOBII_WEARABLE_TRACKING_IMPROVEMENT_INCREASE_EYE_RELIEF,
} tobii_wearable_tracking_improvement_t;

typedef struct tobii_wearable_eye_t
{
    tobii_validity_t gaze_origin_validity;
    float gaze_origin_mm_xyz[ 3 ];

    tobii_validity_t gaze_direction_validity;
    float gaze_direction_normalized_xyz[ 3 ];

    tobii_validity_t pupil_diameter_validity;
    float pupil_diameter_mm;

    tobii_validity_t eye_openness_validity;
    float eye_openness;

    tobii_validity_t pupil_position_in_sensor_area_validity;
    float pupil_position_in_sensor_area_xy[ 2 ];

    tobii_validity_t position_guide_validity;
    float position_guide_xy[ 2 ];
} tobii_wearable_eye_t;

typedef struct tobii_wearable_data_t
{
    int64_t timestamp_tracker_us;
    int64_t timestamp_system_us;
    uint32_t frame_counter;
    uint32_t led_mode;
    tobii_wearable_eye_t left;
    tobii_wearable_eye_t right;

    tobii_validity_t gaze_origin_combined_validity;
    float gaze_origin_combined_mm_xyz[ 3 ];
    tobii_validity_t gaze_direction_combined_validity;
    float gaze_direction_combined_normalized_xyz[ 3 ];
    tobii_validity_t convergence_distance_validity;
    float convergence_distance_mm;
    int tracking_improvements_count;
    tobii_wearable_tracking_improvement_t tracking_improvements[ 10 ];
} tobii_wearable_data_t;

void wearable_callback( tobii_wearable_data_t const* data, void* user_data )
{
    std::cout << "Got Data: " << data->left.eye_openness << "\n";
}

typedef struct tobii_license_key_t
{
    uint16_t const* license_key;
    size_t size_in_bytes;
} tobii_license_key_t;

typedef enum tobii_license_validation_result_t
{
    TOBII_LICENSE_VALIDATION_RESULT_OK,
    TOBII_LICENSE_VALIDATION_RESULT_TAMPERED,
    TOBII_LICENSE_VALIDATION_RESULT_INVALID_APPLICATION_SIGNATURE,
    TOBII_LICENSE_VALIDATION_RESULT_NONSIGNED_APPLICATION,
    TOBII_LICENSE_VALIDATION_RESULT_EXPIRED,
    TOBII_LICENSE_VALIDATION_RESULT_PREMATURE,
    TOBII_LICENSE_VALIDATION_RESULT_INVALID_PROCESS_NAME,
    TOBII_LICENSE_VALIDATION_RESULT_INVALID_SERIAL_NUMBER,
    TOBII_LICENSE_VALIDATION_RESULT_INVALID_MODEL,
} tobii_license_validation_result_t;

typedef void(*functionPointer)(char const* url, void* user_data);
typedef void(*functionPointer2)(tobii_wearable_data_t const* data, void* user_data);
typedef void(*functionPointer3)(void*, INT64);

typedef tobii_error_t (*tobii_api_create)(void*, void*, void*);
typedef tobii_error_t (*tobii_get_api_version)(tobii_version_t*);
typedef tobii_error_t (*tobii_enumerate_local_device_urls)(void*, functionPointer, char*);
typedef tobii_error_t (*tobii_create_device)(void* api, char const* url, void* device);
typedef tobii_error_t (*tobii_get_device_info)(void*, tobii_device_info_t*);
typedef tobii_error_t (*tobii_wearable_data_subscribe)(void*, functionPointer2, INT64*);
typedef tobii_error_t (*tobii_wearable_image_subscribe)(void*, functionPointer3, INT64*);
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
tobii_wearable_image_subscribe image_subscribe_sig;
tobii_wearable_data_subscribe wearable_data_sig;
tobii_wait_for_callbacks tobii_wait_for_callbacks_sig;
tobii_device_process_callbacks tobii_device_process_callbacks_sig;

int main()
{
    std::string tobiiStreamEngineDll = "C:\\Program Files\\VIVE\\SRanipal\\eye_camera_device\\vive_pro_eye\\tobii_stream_engine.dll";

    HINSTANCE tobiiStreamEngine = LoadLibrary(s2ws(tobiiStreamEngineDll).c_str());

    if (!tobiiStreamEngine)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    std::string usbDevConnectDll = "C:\\Program Files\\VIVE\\SRanipal\\eye_camera_device\\vive_pro_eye\\USBDevConnectDLL.dll";
    
    HINSTANCE usbDevConnect = LoadLibrary(s2ws(usbDevConnectDll).c_str());

    if (!usbDevConnect)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    std::string libEChipConnectionDll = "C:\\Program Files\\VIVE\\SRanipal\\eye_camera_device\\vive_pro_eye\\libEChipConnection.dll";
    
    HINSTANCE libEChip = LoadLibrary(s2ws(libEChipConnectionDll).c_str());

    if (!libEChip)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    std::string aaa = "C:\\Program Files\\VIVE\\SRanipal\\eye_camera_device\\vive_pro_eye\\EyeDeviceConfiguration.dll";
    
    HINSTANCE eee = LoadLibrary(s2ws(aaa).c_str());

    if (!eee)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    std::string bbb = "C:\\Program Files\\VIVE\\SRanipal\\opencv_world310.dll";
    
    HINSTANCE fff = LoadLibrary(s2ws(bbb).c_str());

    if (!fff)
    {
        std::cerr << "Failed to load DLL" << std::endl;
        return EXIT_FAILURE;
    }

    std::string eyeDeviceConfigDLL = "C:\\Program Files\\VIVE\\SRanipal\\eye_camera_device\\vive_pro_eye\\EyeDeviceConfiguration.dll";
    
    HINSTANCE eyeDeviceConfig = LoadLibrary(s2ws(eyeDeviceConfigDLL).c_str());

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
    image_subscribe_sig = (tobii_wearable_image_subscribe)GetProcAddress(tobiiStreamEngine, "tobii_wearable_limited_image_subscribe");
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
    }
    
    return 0;
}