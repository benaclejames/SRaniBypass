#pragma once
#include <cstdint>

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