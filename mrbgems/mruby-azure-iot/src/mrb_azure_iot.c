#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <azure_c_shared_utility/const_defines.h>
#include <azure_c_shared_utility/macro_utils.h>
#include <azure_c_shared_utility/shared_util_options.h>
#include <azure_c_shared_utility/umock_c_prod.h>
#include <azure_prov_client/iothub_security_factory.h>
#include <azure_prov_client/prov_device_ll_client.h>
#include <azureiot/azure_sphere_provisioning.h>
#include <azureiot/iothub.h>
#include <azureiot/iothub_client_core_common.h>
#include <azureiot/iothub_client_options.h>
#include <azureiot/iothub_device_client_ll.h>
#include <azureiot/iothub_message.h>
#include <azureiot/iothub_transport_ll.h>
#include <azureiot/iothubtransportmqtt.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/dump.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/value.h>
#include <mruby/variable.h>

#ifdef _MSC_VER
extern int sprintf_s(char *dst, size_t dstSizeInBytes, const char *format, ...);
#endif // _MSC_VER

char *scopeId = NULL;
static const int keepalivePeriodSeconds = 20;

struct RClass *_class_device_client;
struct RClass *_class_message;

static void mrb_device_client_free(mrb_state *mrb, void *ptr);
static void mrb_message_free(mrb_state *mrb, void *ptr);

const static struct mrb_data_type mrb_device_client_type = {"DeviceClient", mrb_device_client_free};
const static struct mrb_data_type mrb_message_type = {"Message", mrb_message_free};

typedef struct DEVICE_CLIENT_CONTEXT
{
	mrb_state *mrb;
	mrb_value self;
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle;
	mrb_value twin;
} DEVICE_CLIENT_CONTEXT;

static IOTHUBMESSAGE_DISPOSITION_RESULT mrb_receive_message_callback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)userContextCallback;
	const unsigned char *buffer;
	size_t size;

	if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		printf("unable to retrieve the message data\r\n");
	}

	printf(buffer);
	printf("\r\n");

	return IOTHUBMESSAGE_ACCEPTED;
}

mrb_value mrb_device_client_initialize(mrb_state *mrb, mrb_value self)
{
	DEVICE_CLIENT_CONTEXT *context = mrb_malloc(mrb, sizeof(DEVICE_CLIENT_CONTEXT));
	if (context == NULL)
	{
		mrb_raise(mrb, E_RUNTIME_ERROR, "DeviceClient.new context");
		return mrb_nil_value();
	}

	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle;
	AZURE_SPHERE_PROV_RETURN_VALUE provResult =
		IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(scopeId, 10000,
																		  &iotHubClientHandle);
	if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK)
	{
		mrb_free(mrb, context);
		mrb_raise(mrb, E_RUNTIME_ERROR, "DeviceClient.new ConnectionString");
		return mrb_nil_value();
	}

	context->mrb = mrb;
	context->self = self;
	context->iotHubClientHandle = iotHubClientHandle;

	mrb_data_init(self, context, &mrb_device_client_type);

	if (IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_KEEP_ALIVE,
										&keepalivePeriodSeconds) != IOTHUB_CLIENT_OK)
	{
		mrb_raise(mrb, E_RUNTIME_ERROR, "ERROR: Failure setting Azure IoT Hub client option \"OPTION_KEEP_ALIVE\".\n");
	}

	if (IoTHubDeviceClient_LL_SetMessageCallback(iotHubClientHandle, mrb_receive_message_callback, context) != IOTHUB_CLIENT_OK)
	{
		mrb_raise(mrb, E_RUNTIME_ERROR, "ERROR: IoTHubDeviceClient_LL_SetMessageCallback..........FAILED!\r\n");
	}

	return self;
}

void mrb_device_client_free(mrb_state *mrb, void *ptr)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)ptr;
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = context->iotHubClientHandle;

	IoTHubDeviceClient_LL_Destroy(iotHubClientHandle);
	mrb_free(mrb, context);
}

typedef struct SEND_CALLBAK_CONTEXT
{
	mrb_state *mrb;
	mrb_value message;

} SEND_CALLBAK_CONTEXT;

static void mrb_send_confirmation_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
	SEND_CALLBAK_CONTEXT *context = (SEND_CALLBAK_CONTEXT *)userContextCallback;
	mrb_state *mrb = context->mrb;
	mrb_value block;

	block = mrb_iv_get(mrb, context->message, mrb_intern_lit(mrb, "callback"));
	if (!mrb_nil_p(block))
	{
		mrb_yield(mrb, block, mrb_nil_value());
	}

	IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle = (IOTHUB_MESSAGE_HANDLE)DATA_PTR(context->message);

	if (ioTHubMessageHandle != NULL)
	{
		IoTHubMessage_Destroy(ioTHubMessageHandle);
		DATA_PTR(context->message) = NULL;
	}

	mrb_free(context->mrb, context);
}

mrb_value mrb_device_client_send_event(mrb_state *mrb, mrb_value self)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)DATA_PTR(self);
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = context->iotHubClientHandle;
	mrb_value message, block = mrb_nil_value();

	mrb_get_args(mrb, "o|&", &message, &block);

	if (!mrb_obj_is_kind_of(mrb, message, _class_message))
	{
		printf("DeviceClient#send_event\n");
		return mrb_nil_value();
	}

	IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle = (IOTHUB_MESSAGE_HANDLE)DATA_PTR(message);
	SEND_CALLBAK_CONTEXT *callbakContext = mrb_malloc(mrb, sizeof(SEND_CALLBAK_CONTEXT));
	callbakContext->mrb = mrb;
	callbakContext->message = message;

	mrb_iv_set(mrb, message, mrb_intern_lit(mrb, "callback"), block);

	IOTHUB_CLIENT_RESULT result;
	result = IoTHubDeviceClient_LL_SendEventAsync(iotHubClientHandle, ioTHubMessageHandle, mrb_send_confirmation_callback, callbakContext);
	if (result != IOTHUB_CLIENT_OK)
	{
		printf("DeviceClient#send_event\n");
		return mrb_nil_value();
	}

	return mrb_true_value();
}

mrb_value mrb_device_client_do_work(mrb_state *mrb, mrb_value self)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)DATA_PTR(self);
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = context->iotHubClientHandle;

	IoTHubDeviceClient_LL_DoWork(iotHubClientHandle);

	return mrb_nil_value();
}

static int mrb_receive_device_method_callback(const char *method_name, const unsigned char *payload, size_t size, unsigned char **response, size_t *resp_size, void *userContextCallback)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)userContextCallback;
	mrb_state *mrb = context->mrb;
	mrb_value twin = context->twin;
	mrb_value arg = mrb_str_new(mrb, payload, size);
	mrb_value ret;

	ret = mrb_funcall(mrb, twin, method_name, 1, arg);
	if (!mrb_string_p(ret))
	{
		ret = mrb_obj_as_string(mrb, ret);
	}

	*resp_size = RSTRING_LEN(ret);
	*response = (unsigned char *)malloc(*resp_size);
	(void)memcpy(*response, RSTRING_PTR(ret), *resp_size);

	return 0;
}

static void mrb_device_twin_callback(int status_code, void *userContextCallback)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)userContextCallback;
}

static void mrb_receive_device_twin_callback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char *payload, size_t size, void *userContextCallback)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)userContextCallback;
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = context->iotHubClientHandle;
	mrb_state *mrb = context->mrb;
	mrb_value twin = context->twin;
	mrb_value arg = mrb_str_new(mrb, payload, size);
	mrb_value ret;

	ret = mrb_funcall(mrb, twin, "recv_twin", 1, arg);
	if (!mrb_string_p(ret))
	{
		ret = mrb_obj_as_string(mrb, ret);
	}

	if (IoTHubDeviceClient_LL_SendReportedState(iotHubClientHandle, RSTRING_PTR(ret), RSTRING_LEN(ret), mrb_device_twin_callback, context) != IOTHUB_CLIENT_OK)
	{
		printf("Failure sending data\n");
	}
}

mrb_value mrb_device_client_set_twin(mrb_state *mrb, mrb_value self)
{
	DEVICE_CLIENT_CONTEXT *context = (DEVICE_CLIENT_CONTEXT *)DATA_PTR(self);
	IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle = context->iotHubClientHandle;
	mrb_value twin;

	mrb_get_args(mrb, "o", &twin);

	context->twin = twin;

	if (IoTHubDeviceClient_LL_SetDeviceMethodCallback(iotHubClientHandle, mrb_receive_device_method_callback, context) != IOTHUB_CLIENT_OK)
	{
		printf("ERROR: IoTHubDeviceClient_LL_SetDeviceMethodCallback..........FAILED!\n");
	}

	if (IoTHubDeviceClient_LL_SetDeviceTwinCallback(iotHubClientHandle, mrb_receive_device_twin_callback, context) != IOTHUB_CLIENT_OK)
	{
		printf("failure in IoTHubDeviceClient_LL_SetDeviceTwinCallback\n");
	}

	return mrb_nil_value();
}

mrb_value mrb_message_initialize(mrb_state *mrb, mrb_value self)
{
	unsigned char *msgText;
	mrb_int msgSize;

	mrb_get_args(mrb, "s", &msgText, &msgSize);

	IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle;
	ioTHubMessageHandle = IoTHubMessage_CreateFromByteArray(msgText, msgSize);
	if (ioTHubMessageHandle == NULL)
	{
		printf("Message.new\n");
		return mrb_nil_value();
	}

	mrb_data_init(self, ioTHubMessageHandle, &mrb_message_type);

	return self;
}

void mrb_message_free(mrb_state *mrb, void *ptr)
{
	IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle = (IOTHUB_MESSAGE_HANDLE)ptr;

	if (ioTHubMessageHandle != NULL)
		IoTHubMessage_Destroy(ioTHubMessageHandle);
}

mrb_value mrb_message_add_property(mrb_state *mrb, mrb_value self)
{
	IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle = (IOTHUB_MESSAGE_HANDLE)DATA_PTR(self);
	const char *name, *value;

	mrb_get_args(mrb, "zz", &name, &value);

	if (IoTHubMessage_SetProperty(ioTHubMessageHandle, name, value) != IOTHUB_MESSAGE_OK)
	{
		printf("Message#add_property\n");
		return mrb_nil_value();
	}

	return mrb_nil_value();
}

void mrb_mruby_azure_iot_gem_init(mrb_state *mrb)
{
	struct RClass *_module_azure_iot;

	_module_azure_iot = mrb_define_module(mrb, "AzureIoT");

	_class_device_client = mrb_define_class_under(mrb, _module_azure_iot, "DeviceClient", mrb->object_class);
	MRB_SET_INSTANCE_TT(_class_device_client, MRB_TT_DATA);
	mrb_define_method(mrb, _class_device_client, "initialize", mrb_device_client_initialize, MRB_ARGS_NONE());
	mrb_define_method(mrb, _class_device_client, "send_event", mrb_device_client_send_event, MRB_ARGS_REQ(2));
	mrb_define_method(mrb, _class_device_client, "do_work", mrb_device_client_do_work, MRB_ARGS_NONE());
	mrb_define_method(mrb, _class_device_client, "set_twin", mrb_device_client_set_twin, MRB_ARGS_REQ(1));

	_class_message = mrb_define_class_under(mrb, _module_azure_iot, "Message", mrb->object_class);
	MRB_SET_INSTANCE_TT(_class_message, MRB_TT_DATA);
	mrb_define_method(mrb, _class_message, "initialize", mrb_message_initialize, MRB_ARGS_REQ(1));
	mrb_define_method(mrb, _class_message, "add_property", mrb_message_add_property, MRB_ARGS_REQ(2));
}

void mrb_mruby_azure_iot_gem_final(mrb_state *mrb)
{
}
