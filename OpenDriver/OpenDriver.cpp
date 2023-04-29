#include <ntddk.h>
void OnUnLoad(IN PDRIVER_OBJECT DriverObject) {
	DriverObject = DriverObject;
	UNICODE_STRING symbolicLinkName;
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\OpenDriver");// symbolicName 초기화


	IoDeleteSymbolicLink(&symbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);
	
	DbgPrint("Unloaded called\n");
}
typedef struct
{
	unsigned char Buffer[4];
	int DataSize;
}DEVICE_EXTENSION;

NTSTATUS MyCreateDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pDevObj = pDevObj;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, 0); // 0 -> IO_NO_INCREMENT 
	return STATUS_SUCCESS;
}
NTSTATUS MyCloseDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pDevObj = pDevObj;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	
	IoCompleteRequest(pIrp, 0); // 0 -> IO_NO_INCREMENT 
	return STATUS_SUCCESS;
}
NTSTATUS MyReadDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pDevObj = pDevObj;

	PIO_STACK_LOCATION pIoStackLocation;
	int Length;
	unsigned char* pUserBuffer=NULL;
	DEVICE_EXTENSION* pDevExt;
	pDevExt = (DEVICE_EXTENSION*)pDevObj->DeviceExtension;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	Length = pIoStackLocation->Parameters.Read.Length;
	if (Length > pDevExt->DataSize) Length = pDevExt->DataSize;

	memcpy_s(pUserBuffer, Length, pDevExt->Buffer, Length);
	pDevExt->DataSize = Length;

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = Length;//얼마나 수행되었는지
	IoCompleteRequest(pIrp, 0); // 0 -> IO_NO_INCREMENT 
	return STATUS_SUCCESS;
}
NTSTATUS MyWriteDispatch(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	PIO_STACK_LOCATION pIoStackLocation;
	int Length;
	unsigned char* pUserBuffer;
	DEVICE_EXTENSION* pDevExt;
	pDevObj = pDevObj;

	
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	Length = pIoStackLocation->Parameters.Write.Length;
	if (Length > 4) Length = 4;
	pUserBuffer =(unsigned char *) pIrp->UserBuffer;
	pDevExt = (DEVICE_EXTENSION*)pDevObj->DeviceExtension;
	
	memcpy_s(pDevExt->Buffer, Length, pUserBuffer, Length);
	pDevExt->DataSize = Length;

	pIrp->IoStatus.Information = Length; //얼마나 수행되었는지

	pIrp->IoStatus.Status = STATUS_SUCCESS; 
	IoCompleteRequest(pIrp, 0); // 0 -> IO_NO_INCREMENT 
	return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegistryPath)
{

	PDEVICE_OBJECT pDeviceObj = NULL;
	NTSTATUS nStatus;
	UNICODE_STRING deviceName;
	UNICODE_STRING symbolicLinkName;
	DEVICE_EXTENSION* pDevExt;
	pDrvObj = pDrvObj;
	pRegistryPath = pRegistryPath;
	pDrvObj->DriverUnload = OnUnLoad;
	pDrvObj->MajorFunction[IRP_MJ_CREATE] = MyCreateDispatch;
	pDrvObj->MajorFunction[IRP_MJ_CLOSE] = MyCloseDispatch;
	pDrvObj->MajorFunction[IRP_MJ_READ] = MyReadDispatch;
	pDrvObj->MajorFunction[IRP_MJ_WRITE] = MyWriteDispatch;
	
	RtlInitUnicodeString(&deviceName, L"\\Device\\OpenTDriver");//Initialize 드라이버 이름 초기화
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\OpenDriver");// symbolicName 초기화
	nStatus = IoCreateDevice(pDrvObj, sizeof(DEVICE_EXTENSION), &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObj);// Device Object Name 추가 작업
	pDevExt = (DEVICE_EXTENSION*)pDeviceObj->DeviceExtension;
	pDevExt->DataSize = 0;
	
	if (NT_SUCCESS(nStatus))
	{
		nStatus = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
		if (NT_SUCCESS(nStatus))
			DbgPrint("success createsymbolic\n");

		
	}

	DbgPrint("Hello World!\n");
	return STATUS_SUCCESS;

}