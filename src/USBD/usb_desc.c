#include "ch32v30x.h"
#include "ch32v30x_usb.h"
#include "ch32v30x_usbhs_device.h"

#include "usb_desc.h"


/* Device Descriptor */
const uint8_t USB_DevDesc[] =
{
    USB_SIZE_DEVICE_DESC,               // bLength
    USB_DESCR_TYP_DEVICE,               // bDescriptorType
    0x00, 0x02,                         // bcdUSB
    0x00,                               // bDeviceClass
    0x00,                               // bDeviceSubClass
    0x00,                               // bDeviceProtocol
    USB_MAX_EP0_SZ,                     // bMaxPacketSize0
    USBD_VID & 0xFF, USBD_VID >> 8,     // idVendor
    USBD_PID & 0xFF, USBD_PID >> 8,     // idProduct
    0x10, 0x01,                         // bcdDevice
    0x01,                               // iManufacture
    0x02,                               // iProduct
    0x03,                               // iSerialNumber
    0x01                                // bNumConfigurations
};


/* Configuration Descriptor (FS) */
const uint8_t USB_CfgDesc_FS[] =
{
    USB_SIZE_CONFIG_DESC,               // bLength
    USB_DESCR_TYP_CONFIG,               // bDescriptorType
    USB_SIZE_CONFIG_TOTAL & 0xFF,
    USB_SIZE_CONFIG_TOTAL >> 8,         // wTotalLength
    2,                                  // bNumInterfaces
    0x01,                               // bConfigurationValue
    0x00,                               // iConfiguration
    0x00,                               // bmAttributes, D6: self power  D5: remote wake-up
    0x64,                               // MaxPower, 100 * 2mA = 200mA

    // Interface Association Descriptor (IAD)
    USB_SIZE_IAD_DESC,                  // bLength
    0x0B,                               // bDescriptorType: IAD
    0x01,                               // bFirstInterface
    0x02,                               // bInterfaceCount
    0x02,                               // bFunctionClass: CDC
    0x02,                               // bFunctionSubClass
    0x01,                               // bFunctionProtocol
    0x00,                               // iFunction

    // I/F descriptor: CDC Control
    USB_SIZE_INTERFACE_DESC,            // bLength
    USB_DESCR_TYP_INTERF,               // bDescriptorType
    0x01,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x01,                               // bNumEndpoints
    0x02,                               // bInterfaceClass
    0x02,                               // bInterfaceSubClass
    0x01,                               // bInterfaceProtocol
    0x00,                               // iInterface

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x00,                               // Header functional descriptor subtype
    0x10, 0x01,                         // Communication device compliant to the communication spec. ver. 1.10

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x01,                               // Call management functional descriptor
    0x00,                               // BIT0: Whether device handle call management itself.
                                        // BIT1: Whether device can send/receive call management information over a Data Class Interface 0
    0x02,                               // Interface number of data class interface optionally used for call management

    // Communication Class Specified INTERFACE descriptor
    0x04,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x02,                               // Abstract control management functional descriptor subtype
    0x00,                               // bmCapabilities

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // bLength
    0x24,                               // bDescriptorType: CS_INTERFACE descriptor type
    0x06,                               // bDescriptorSubType
    0x01,                               // bMasterInterface
    0x02,                               // bSlaveInterface0

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_INT_IN_EP,                      // bEndpointAddress
    USB_EPT_INTERRUPT,                  // bmAttributes
    CDC_INT_IN_SZ_FS, 0x00,             // wMaxPacketSize
    10,                                 // bInterval

    // I/F descriptor: CDC Data
    USB_SIZE_INTERFACE_DESC,            // bLength
    USB_DESCR_TYP_INTERF,               // bDescriptorType
    0x02,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x02,                               // bNumEndpoints
    0x0A,                               // bInterfaceClass
    0x00,                               // bInterfaceSubClass
    0x00,                               // bInterfaceProtocol
    0x00,                               // iInterface

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_BULK_IN_EP,                     // bEndpointAddress
    USB_EPT_BULK,                       // bmAttributes
    CDC_BULK_IN_SZ_FS, 0x00,            // wMaxPacketSize
    0x00,                               // bInterval

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_BULK_OUT_EP,                    // bEndpointAddress
    USB_EPT_BULK,                       // bmAttributes
    CDC_BULK_OUT_SZ_FS, 0x00,           // wMaxPacketSize
    0x00,                               // bInterval
};


/* Configuration Descriptor (HS) */
const uint8_t USB_CfgDesc_HS[] =
{
    USB_SIZE_CONFIG_DESC,               // bLength
    USB_DESCR_TYP_CONFIG,               // bDescriptorType
    USB_SIZE_CONFIG_TOTAL & 0xFF,
    USB_SIZE_CONFIG_TOTAL >> 8,         // wTotalLength
    2,                                  // bNumInterfaces
    0x01,                               // bConfigurationValue
    0x00,                               // iConfiguration
    0x00,                               // bmAttributes, D6: self power  D5: remote wake-up
    0x64,                               // MaxPower, 100 * 2mA = 200mA

    // Interface Association Descriptor (IAD)
    USB_SIZE_IAD_DESC,                  // bLength
    0x0B,                               // bDescriptorType: IAD
    0x01,                               // bFirstInterface
    0x02,                               // bInterfaceCount
    0x02,                               // bFunctionClass: CDC
    0x02,                               // bFunctionSubClass
    0x01,                               // bFunctionProtocol
    0x00,                               // iFunction

    // I/F descriptor: CDC Control
    USB_SIZE_INTERFACE_DESC,            // bLength
    USB_DESCR_TYP_INTERF,               // bDescriptorType
    0x01,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x01,                               // bNumEndpoints
    0x02,                               // bInterfaceClass
    0x02,                               // bInterfaceSubClass
    0x01,                               // bInterfaceProtocol
    0x00,                               // iInterface

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x00,                               // Header functional descriptor subtype
    0x10, 0x01,                         // Communication device compliant to the communication spec. ver. 1.10

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x01,                               // Call management functional descriptor
    0x00,                               // BIT0: Whether device handle call management itself.
                                        // BIT1: Whether device can send/receive call management information over a Data Class Interface 0
    0x02,                               // Interface number of data class interface optionally used for call management

    // Communication Class Specified INTERFACE descriptor
    0x04,                               // Size of the descriptor, in bytes
    0x24,                               // CS_INTERFACE descriptor type
    0x02,                               // Abstract control management functional descriptor subtype
    0x00,                               // bmCapabilities

    // Communication Class Specified INTERFACE descriptor
    0x05,                               // bLength
    0x24,                               // bDescriptorType: CS_INTERFACE descriptor type
    0x06,                               // bDescriptorSubType
    0x01,                               // bMasterInterface
    0x02,                               // bSlaveInterface0

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_INT_IN_EP,                      // bEndpointAddress
    USB_EPT_INTERRUPT,                  // bmAttributes
    CDC_INT_IN_SZ_HS & 0xFF,
    CDC_INT_IN_SZ_HS >> 8,              // wMaxPacketSize
    10,                                 // bInterval

    // I/F descriptor: CDC Data
    USB_SIZE_INTERFACE_DESC,            // bLength
    USB_DESCR_TYP_INTERF,               // bDescriptorType
    0x02,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x02,                               // bNumEndpoints
    0x0A,                               // bInterfaceClass
    0x00,                               // bInterfaceSubClass
    0x00,                               // bInterfaceProtocol
    0x00,                               // iInterface

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_BULK_IN_EP,                     // bEndpointAddress
    USB_EPT_BULK,                       // bmAttributes
    CDC_BULK_IN_SZ_HS & 0xFF,
    CDC_BULK_IN_SZ_HS >> 8,             // wMaxPacketSize
    0x00,                               // bInterval

    // ENDPOINT descriptor
    USB_SIZE_ENDPOINT_DESC,             // bLength
    USB_DESCR_TYP_ENDP,                 // bDescriptorType
    CDC_BULK_OUT_EP,                    // bEndpointAddress
    USB_EPT_BULK,                       // bmAttributes
    CDC_BULK_OUT_SZ_HS & 0xFF,
    CDC_BULK_OUT_SZ_HS >> 8,            // wMaxPacketSize
    0x00,                               // bInterval
};


/* Language Descriptor */
const uint8_t USB_StringLangID[] =
{
    USB_SIZE_STRING_LANGID,
    USB_DESCR_TYP_STRING,
    0x09, 0x04
};


/* Manufacturer Descriptor */
const uint8_t USB_StringVendor[] =
{
    USB_SIZE_STRING_VENDOR,
    USB_DESCR_TYP_STRING,
    'X', 0, 'I', 0, 'V', 0, 'N', 0, '1', 0, '9', 0, '8', 0, '7', 0
};


/* Product Descriptor */
const uint8_t USB_StringProduct[] =
{
    USB_SIZE_STRING_PRODUCT,
    USB_DESCR_TYP_STRING,
    'X', 0, 'V', 0, '-', 0, 'U', 0, 'S', 0, 'B', 0, '2', 0, 'S', 0, 'P', 0, 'I', 0
};


/* Serial Number Descriptor */
const uint8_t USB_StringSerialNbr[] =
{
    USB_SIZE_STRING_SERIALNBR,
    USB_DESCR_TYP_STRING,
    '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0, 'A', 0, 'B', 0
};


/* Device Qualified Descriptor */
const uint8_t USB_QualifierDesc[] =
{
    USB_SIZE_QUALIFIER_DESC,    // bLength
    USB_DESCR_TYP_QUALIF,       // bDescriptorType
    0x00, 0x02,                 // bcdUSB
    0xFF,                       // bDeviceClass
    0xFF,                       // bDeviceSubClass
    0xFF,                       // bDeviceProtocol
    0x40,                       // bMaxPacketSize0 for other speed
    0x01,                       // bNumConfigurations
    0x00,                       // bReserved
};
