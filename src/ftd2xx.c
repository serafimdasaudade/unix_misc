

#include <usb.h>
#include <ftdi.h>

#include "ftd2xx.h"
#include "wine/debug.h"
WINE_DEFAULT_DEBUG_CHANNEL(ftd2xx);


#ifndef ftdi_error_return

#define ftdi_error_return(code, str) do {  \
        ftdi->error_str = str;             \
        return code;                       \
   } while(0);    
#endif

FT_STATUS WINAPI FT_ListDevices(PVOID pArg1, PVOID pArg2, DWORD flags) {
	
	struct ftdi_context ftdi;
	if (ftdi_init(&ftdi) < 0) {
		WARN("Failed to initalise the FTDI Device (%s)\n", ftdi_get_error_string(&ftdi));
		return FT_DEVICE_NOT_FOUND;
	}
	
	struct ftdi_device_list *buf = NULL;	/* Stores the devices */
	DWORD nu = 0;							/* Stores the number of devices */
	
	FT_STATUS r = FT_OK;					/* Stores the return code */
	nu = ftdi_usb_find(&ftdi, &buf);		/* Get the USB devices */
	
	/* We didn't find any devices */
	if (buf == NULL) return FT_DEVICE_NOT_FOUND;
	
	if ((flags & FT_LIST_NUMBER_ONLY) == FT_LIST_NUMBER_ONLY) {
		/* Return the number of USB devices in pArg1 */
		memcpy(pArg1, &nu, sizeof(DWORD));

		TRACE("FT_LIST_NUMBER_ONLY %li %p (%li)\n", nu, pArg1, *(DWORD*)pArg1);
	} else if ((flags & FT_LIST_ALL) == FT_LIST_ALL) {
		TRACE("Doing FT_LIST_ALL\n");

		/* Return an array of (char*[]) description strings in pArg1 */
		if ((flags & FT_OPEN_BY_DESCRIPTION) == FT_OPEN_BY_DESCRIPTION) {
			
		/* Return an array of (long[]) Location IDs in pArg1 */
		} else if ((flags & FT_OPEN_BY_LOCATION) == FT_OPEN_BY_LOCATION) {
			
		/* Return an array of (char*[]) serial number string in pArg1 */
		} else {
			
		}
		
	} else {
		DWORD index = *((DWORD*)pArg1);
		TRACE("Doing FT_LIST_BY_INDEX %ld\n", index);
		
		if (index >= nu) {
			r = FT_DEVICE_NOT_FOUND;
			goto end;
		} else
			nu = 0;
		
		/* Put the correct device into buffer */
		struct ftdi_device_list* node;
		for (node = buf; nu < index; node = node->next)
			nu++;

		struct usb_device* dev = node->dev;
		struct usb_dev_handle* handle;
		if (!(handle = usb_open(dev))) {
			WARN("Opening USB device (%ld) failed\n", nu);
			r = FT_DEVICE_NOT_OPENED;
			goto end;
		}	
		
		/* Return (char*) product description string in pArg2 */
		if ((flags & FT_OPEN_BY_DESCRIPTION) == FT_OPEN_BY_DESCRIPTION) {
			TRACE("Doing FT_OPEN_BY_DESCRIPTION");
			if (usb_get_string_simple(handle, dev->descriptor.iProduct, (char*)pArg2, 64) >= 0)
				r = FT_IO_ERROR;
			
		/* Return (long) Location ID in pArg2 */
		} else if ((flags & FT_OPEN_BY_LOCATION) == FT_OPEN_BY_LOCATION) {
			TRACE("Doing FT_OPEN_BY_LOCATION\n");
			FIXME("FT_OPEN_BY_LOCATION call should return location (just returns index currently)...\n");
			memcpy(pArg2, &nu, sizeof(long));
		
		/* Return (char*) serial number string in pArg2 */
		} else {
			TRACE("Doing FT_OPEN_BY_SERIAL_NUMBER\n");
			if (usb_get_string_simple(handle, dev->descriptor.iSerialNumber, (char*)pArg2, 64) >= 0)
				r = FT_IO_ERROR;
		}

		usb_close (handle);
	}

	
end:
	ftdi_list_free(&buf);

	TRACE("FT_ListDevices Return %p %p %lx\n", pArg1, pArg2, r);
	return r;
}

FT_STATUS WINAPI FT_Open(int deviceNumber, FT_HANDLE* pHandle) {
	TRACE("FT_Open Call %x %p (%p)\n", deviceNumber, pHandle, *pHandle);
	
	struct ftdi_device_list *buf = NULL;	/* Stores the devices */
	DWORD nu = 0;							/* Stores the number of devices */
	
	FT_STATUS r = FT_OK;					/* Stores the return code */

	/* Create a context */
	struct ftdi_context* ftdi = (struct ftdi_context*)malloc(sizeof(struct ftdi_context));
	if(ftdi_init(ftdi) < 0) {
		WARN("Failed to initalise the FTDI Device (%s)\n", ftdi_get_error_string(ftdi));
		r = FT_IO_ERROR;
		goto end;
	}
	
	nu = ftdi_usb_find(ftdi, &buf);				/* Get the USB devices */
	if (deviceNumber >= nu) {
		r = FT_DEVICE_NOT_FOUND;
		goto end;
	} else
		nu = 0;

	/* Put the correct device into buffer */
	struct ftdi_device_list* node;
	for (node = buf; nu < deviceNumber; node = node->next)
		nu++;
	
	/* Open the device */
	if(ftdi_usb_open_dev(ftdi, node->dev) < 0) {
		WARN("Failed to open the FTDI Device (%s)\n", ftdi_get_error_string(ftdi));
		r = FT_IO_ERROR;
		goto end;
	}

	/* Set a nice fast baud rate */
	if(ftdi_set_baudrate(ftdi, 1228800) < 0) {
		WARN("Failed to open the FTDI Device (%s)\n", ftdi_get_error_string(ftdi));
		r = FT_IO_ERROR;
		goto end;
	}

	WARN("Baudrate is now %i\n", ftdi->baudrate);
	
	*pHandle = ftdi;

end:
	ftdi_list_free(&buf);

	TRACE("FT_Open Return %p (%p) %lx\n", pHandle, *pHandle, r);
	return r;
	
}

FT_STATUS WINAPI FT_Close(FT_HANDLE ftHandle) {
	TRACE("FT_Close Call %p\n", ftHandle);
	
	if (ftdi_usb_close((struct ftdi_context*)ftHandle) < 0) {
		WARN("Failed to open the FTDI Device (%s)\n", ftdi_get_error_string((struct ftdi_context*)ftHandle));
		return FT_IO_ERROR;
	}

	return FT_OK;
}

FT_STATUS WINAPI FT_GetDeviceInfo(FT_HANDLE ftHandle, FT_DEVICE *lpftDevice, LPDWORD lpdwID, PCHAR SerialNumber, PCHAR Description,	LPVOID Dummy) {
	TRACE("FT_GetDeviceInfo Call %p %p %p %p %p\n", ftHandle, lpftDevice, lpdwID, SerialNumber, Description);

	struct ftdi_context* ftdi = (struct ftdi_context*)ftHandle;
	struct usb_device* dev = usb_device(ftdi->usb_dev);

	*lpdwID = (dev->descriptor.idVendor << 16) | dev->descriptor.idProduct;
	if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iSerialNumber, SerialNumber, 16) <= 0) {
		WARN("Failed to get the serial number.\n");
		return FT_IO_ERROR;
	}
	TRACE("SerialNumber %s\n", SerialNumber);
	
	if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iProduct, Description, 64) <= 0) {
		WARN("Failed to get the description.\n");
		return FT_IO_ERROR;
	}
	TRACE("Description %s\n", Description);
	
	TRACE("FT_GetDeviceInfo Return %p %p %p %p %p\n", ftHandle, lpftDevice, lpdwID, SerialNumber, Description);
	return FT_OK;
}

FT_STATUS WINAPI FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesReturned) {
	TRACE("FT_Read Call %p %p %ld %p %ld\n", ftHandle, lpBuffer, nBufferSize, lpBytesReturned, *lpBytesReturned);
	
	int r = ftdi_read_data((struct ftdi_context*)ftHandle, lpBuffer, nBufferSize);
	if (r < 0) {
		WARN("Failed to read for data %i (%s)\n", r, ftdi_get_error_string((struct ftdi_context*)ftHandle));
		return FT_IO_ERROR;
	}
		
	*lpBytesReturned = (DWORD)r;
	TRACE("FT_Read Return %p %p %ld %p %ld\n", ftHandle, lpBuffer, nBufferSize, lpBytesReturned, *lpBytesReturned);
	return FT_OK;
}

#define PACKETSIZE 1054
FT_STATUS WINAPI FT_Write(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten) {
	TRACE("FT_Write Call %p %p %ld %p (%ld) \n", ftHandle, lpBuffer, nBufferSize, lpBytesWritten, *lpBytesWritten);

	struct ftdi_context* ftdi = (struct ftdi_context*)ftHandle;
	
	DWORD i, r;
	unsigned char* buf = (unsigned char*)lpBuffer;
	for ( i = 0; i < nBufferSize; i += min(PACKETSIZE, nBufferSize-i)) {
	
		ftdi_poll_read(ftdi, 0);

		TRACE("Writing %ld\n", min(PACKETSIZE, nBufferSize-i));
		r = ftdi_write_data(ftdi, &buf[i], min(PACKETSIZE, nBufferSize-i));
		
		if (r < 0) {
			WARN("Failed to write %ld (%s)\n", r, ftdi_get_error_string(ftdi));
			return FT_IO_ERROR;
		}
	}
	
	*lpBytesWritten = (DWORD)i;
	TRACE("FT_Write Return %p %p %ld %p (%ld) \n", ftHandle, lpBuffer, nBufferSize, lpBytesWritten, *lpBytesWritten);
	return FT_OK;
}

FT_STATUS WINAPI FT_ResetDevice(FT_HANDLE ftHandle) {
	TRACE("FT_ResetDevice Call %p\n", ftHandle);
}

FT_STATUS WINAPI FT_Purge(FT_HANDLE ftHandle, ULONG Mask) {
	TRACE("FT_Purge Call %p %lx\n", ftHandle, Mask);
	
	struct ftdi_context* ftdi = (struct ftdi_context*)ftHandle;
	int rtn = 0;
	
	if ((Mask & FT_PURGE_RX) == FT_PURGE_RX)
		rtn |= ftdi_usb_purge_buffer_read(ftdi);
	
	if ((Mask & FT_PURGE_TX) == FT_PURGE_TX)
		rtn |= ftdi_usb_purge_buffer_write(ftdi);
	
	if (rtn != 0) {
		WARN("Failed to clear buffers (%s)\n", ftdi_get_error_string(ftdi));
		return FT_IO_ERROR;
	}

	TRACE("FT_Purge Return\n");
	return FT_OK;
}

FT_STATUS WINAPI FT_GetQueueStatus(FT_HANDLE ftHandle, DWORD *dwRxBytes) {
	TRACE("FT_GetQueueStatus Call %p %p %ld\n", ftHandle, dwRxBytes, *dwRxBytes);

	struct ftdi_context* ftdi = (struct ftdi_context*)ftHandle;
	ftdi_poll_read(ftdi, 0);
	ftdi_poll_write(ftdi, 0);
	
	
	*dwRxBytes = ftdib_size(ftdi->readbuffer);
	if (*dwRxBytes < 0) {
		WARN("Failed to check for data (%s)\n", ftdi_get_error_string((struct ftdi_context*)ftHandle));
		return FT_IO_ERROR;
	}
		
	TRACE("FT_GetQueueStatus Return %p %p %ld\n", ftHandle, dwRxBytes, *dwRxBytes);
	return FT_OK;
}

FT_STATUS WINAPI FT_GetStatus(FT_HANDLE ftHandle, DWORD *dwRxBytes, DWORD *dwTxBytes, DWORD *dwEventDWord) {
	TRACE("FT_GetStatus Call %p\n", ftHandle);
}

FT_STATUS WINAPI FT_SetLatencyTimer(FT_HANDLE ftHandle, UCHAR ucLatency) {
	TRACE("FT_SetLatencyTimer Call %p\n", ftHandle);
}

FT_STATUS WINAPI FT_SetBitMode(FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable) {
	TRACE("FT_SetBitMode Call %p\n", ftHandle);
}

FT_STATUS WINAPI FT_GetBitMode(FT_HANDLE ftHandle, PUCHAR pucMode) {
	TRACE("FT_GetBitMode Call %p\n", ftHandle);
}

FT_STATUS WINAPI FT_SetUSBParameters(FT_HANDLE ftHandle, ULONG ulInTransferSize, ULONG ulOutTransferSize) {
	TRACE("FT_SetUSBParamters Call %p\n", ftHandle);
}

