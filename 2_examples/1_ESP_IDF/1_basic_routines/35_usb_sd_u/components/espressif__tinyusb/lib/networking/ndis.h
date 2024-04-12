/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file ndis.h ***************************************************************
 *
 * \brief
 *      This file contains the possible external configuration of the USB.
 *
 * \addtogroup usbstick
 *
 *
 ******************************************************************************/

/**
 \ingroup usbstick
 \defgroup RNDIS RNDIS Support
 @{
 */

/*
 * ndis.h
 *
 * Modified by Colin O'Flynn <coflynn@newae.com>
 * ntddndis.h modified by Benedikt Spranger <b.spranger@pengutronix.de>
 *
 * Thanks to the cygwin development team,
 * especially to Casper S. Hornstrup <chorns@users.sourceforge.net>
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 *
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _LINUX_NDIS_H
#define _LINUX_NDIS_H


#define NDIS_STATUS_MULTICAST_FULL          0xC0010009
#define NDIS_STATUS_MULTICAST_EXISTS      0xC001000A
#define NDIS_STATUS_MULTICAST_NOT_FOUND   0xC001000B

/* from drivers/net/sk98lin/h/skgepnmi.h */
#define OID_PNP_CAPABILITIES                    0xFD010100
#define OID_PNP_SET_POWER                       0xFD010101
#define OID_PNP_QUERY_POWER                     0xFD010102
#define OID_PNP_ADD_WAKE_UP_PATTERN             0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN          0xFD010104
#define OID_PNP_ENABLE_WAKE_UP                  0xFD010106

enum NDIS_DEVICE_POWER_STATE {
    NdisDeviceStateUnspecified = 0,
    NdisDeviceStateD0,
    NdisDeviceStateD1,
    NdisDeviceStateD2,
    NdisDeviceStateD3,
    NdisDeviceStateMaximum
};

struct NDIS_PM_WAKE_UP_CAPABILITIES {
    enum NDIS_DEVICE_POWER_STATE  MinMagicPacketWakeUp;
    enum NDIS_DEVICE_POWER_STATE  MinPatternWakeUp;
    enum NDIS_DEVICE_POWER_STATE  MinLinkChangeWakeUp;
};

/* NDIS_PNP_CAPABILITIES.Flags constants */
#define NDIS_DEVICE_WAKE_UP_ENABLE                0x00000001
#define NDIS_DEVICE_WAKE_ON_PATTERN_MATCH_ENABLE  0x00000002
#define NDIS_DEVICE_WAKE_ON_MAGIC_PACKET_ENABLE   0x00000004

/*
struct NDIS_PNP_CAPABILITIES {
    __le32                    Flags;
    struct NDIS_PM_WAKE_UP_CAPABILITIES    WakeUpCapabilities;
};

struct NDIS_PM_PACKET_PATTERN {
    __le32    Priority;
    __le32    Reserved;
    __le32    MaskSize;
    __le32    PatternOffset;
    __le32    PatternSize;
    __le32    PatternFlags;
};
*/

/* Required Object IDs (OIDs) */
#define OID_GEN_SUPPORTED_LIST            0x00010101
#define OID_GEN_HARDWARE_STATUS           0x00010102
#define OID_GEN_MEDIA_SUPPORTED           0x00010103
#define OID_GEN_MEDIA_IN_USE              0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD         0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE        0x00010106
#define OID_GEN_LINK_SPEED                0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE     0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE      0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE       0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE        0x0001010B
#define OID_GEN_VENDOR_ID                 0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION        0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER     0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD         0x0001010F
#define OID_GEN_DRIVER_VERSION            0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE        0x00010111
#define OID_GEN_PROTOCOL_OPTIONS          0x00010112
#define OID_GEN_MAC_OPTIONS               0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS      0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS      0x00010115
#define OID_GEN_VENDOR_DRIVER_VERSION     0x00010116
#define OID_GEN_SUPPORTED_GUIDS           0x00010117
#define OID_GEN_NETWORK_LAYER_ADDRESSES   0x00010118
#define OID_GEN_TRANSPORT_HEADER_OFFSET   0x00010119
#define OID_GEN_MACHINE_NAME              0x0001021A
#define OID_GEN_RNDIS_CONFIG_PARAMETER    0x0001021B
#define OID_GEN_VLAN_ID                   0x0001021C

/* Optional OIDs */
#define OID_GEN_MEDIA_CAPABILITIES        0x00010201
#define OID_GEN_PHYSICAL_MEDIUM           0x00010202

/* Required statistics OIDs */
#define OID_GEN_XMIT_OK                   0x00020101
#define OID_GEN_RCV_OK                    0x00020102
#define OID_GEN_XMIT_ERROR                0x00020103
#define OID_GEN_RCV_ERROR                 0x00020104
#define OID_GEN_RCV_NO_BUFFER             0x00020105

/* Optional statistics OIDs */
#define OID_GEN_DIRECTED_BYTES_XMIT       0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT      0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT      0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT     0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT      0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT     0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV        0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV       0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV       0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV      0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV       0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV      0x0002020C
#define OID_GEN_RCV_CRC_ERROR             0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH     0x0002020E
#define OID_GEN_GET_TIME_CAPS             0x0002020F
#define OID_GEN_GET_NETCARD_TIME          0x00020210
#define OID_GEN_NETCARD_LOAD              0x00020211
#define OID_GEN_DEVICE_PROFILE            0x00020212
#define OID_GEN_INIT_TIME_MS              0x00020213
#define OID_GEN_RESET_COUNTS              0x00020214
#define OID_GEN_MEDIA_SENSE_COUNTS        0x00020215
#define OID_GEN_FRIENDLY_NAME             0x00020216
#define OID_GEN_MINIPORT_INFO             0x00020217
#define OID_GEN_RESET_VERIFY_PARAMETERS   0x00020218

/* IEEE 802.3 (Ethernet) OIDs */
#define NDIS_802_3_MAC_OPTION_PRIORITY    0x00000001

#define OID_802_3_PERMANENT_ADDRESS       0x01010101
#define OID_802_3_CURRENT_ADDRESS         0x01010102
#define OID_802_3_MULTICAST_LIST          0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE       0x01010104
#define OID_802_3_MAC_OPTIONS             0x01010105
#define OID_802_3_RCV_ERROR_ALIGNMENT     0x01020101
#define OID_802_3_XMIT_ONE_COLLISION      0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS    0x01020103
#define OID_802_3_XMIT_DEFERRED           0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS     0x01020202
#define OID_802_3_RCV_OVERRUN             0x01020203
#define OID_802_3_XMIT_UNDERRUN           0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE  0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST     0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS    0x01020207

/* Wireless LAN OIDs */
/* Mandatory */
#define OID_802_11_BSSID                  0x0D010101 /* Q  S     */
#define OID_802_11_SSID                   0x0D010102 /* Q  S     */
#define OID_802_11_NETWORK_TYPE_IN_USE    0x0D010204 /* Q  S     */
#define OID_802_11_RSSI                   0x0D010206 /* Q      I */
#define OID_802_11_BSSID_LIST             0x0D010217 /* Q        */
#define OID_802_11_BSSID_LIST_SCAN        0x0D01011A /*    S     */
#define OID_802_11_INFRASTRUCTURE_MODE    0x0D010108 /* Q  S     */
#define OID_802_11_SUPPORTED_RATES        0x0D01020E /* Q        */
#define OID_802_11_CONFIGURATION          0x0D010211 /* Q  S     */
#define OID_802_11_ADD_WEP                0x0D010113 /*    S     */
#define OID_802_11_WEP_STATUS             0x0D01011B /* Q  S     */
#define OID_802_11_REMOVE_WEP             0x0D010114 /*    S     */
#define OID_802_11_DISASSOCIATE           0x0D010115 /*    S     */
#define OID_802_11_AUTHENTICATION_MODE    0x0D010118 /* Q  S     */
#define OID_802_11_RELOAD_DEFAULTS        0x0D01011C /*    S     */



/* OID_GEN_MINIPORT_INFO constants */
#define NDIS_MINIPORT_BUS_MASTER                      0x00000001
#define NDIS_MINIPORT_WDM_DRIVER                      0x00000002
#define NDIS_MINIPORT_SG_LIST                         0x00000004
#define NDIS_MINIPORT_SUPPORTS_MEDIA_QUERY            0x00000008
#define NDIS_MINIPORT_INDICATES_PACKETS               0x00000010
#define NDIS_MINIPORT_IGNORE_PACKET_QUEUE             0x00000020
#define NDIS_MINIPORT_IGNORE_REQUEST_QUEUE            0x00000040
#define NDIS_MINIPORT_IGNORE_TOKEN_RING_ERRORS        0x00000080
#define NDIS_MINIPORT_INTERMEDIATE_DRIVER             0x00000100
#define NDIS_MINIPORT_IS_NDIS_5                       0x00000200
#define NDIS_MINIPORT_IS_CO                           0x00000400
#define NDIS_MINIPORT_DESERIALIZE                     0x00000800
#define NDIS_MINIPORT_REQUIRES_MEDIA_POLLING          0x00001000
#define NDIS_MINIPORT_SUPPORTS_MEDIA_SENSE            0x00002000
#define NDIS_MINIPORT_NETBOOT_CARD                    0x00004000
#define NDIS_MINIPORT_PM_SUPPORTED                    0x00008000
#define NDIS_MINIPORT_SUPPORTS_MAC_ADDRESS_OVERWRITE  0x00010000
#define NDIS_MINIPORT_USES_SAFE_BUFFER_APIS           0x00020000
#define NDIS_MINIPORT_HIDDEN                          0x00040000
#define NDIS_MINIPORT_SWENUM                          0x00080000
#define NDIS_MINIPORT_SURPRISE_REMOVE_OK              0x00100000
#define NDIS_MINIPORT_NO_HALT_ON_SUSPEND              0x00200000
#define NDIS_MINIPORT_HARDWARE_DEVICE                 0x00400000
#define NDIS_MINIPORT_SUPPORTS_CANCEL_SEND_PACKETS    0x00800000
#define NDIS_MINIPORT_64BITS_DMA                      0x01000000

#define NDIS_MEDIUM_802_3        0x00000000
#define NDIS_MEDIUM_802_5        0x00000001
#define NDIS_MEDIUM_FDDI        0x00000002
#define NDIS_MEDIUM_WAN            0x00000003
#define NDIS_MEDIUM_LOCAL_TALK        0x00000004
#define NDIS_MEDIUM_DIX            0x00000005
#define NDIS_MEDIUM_ARCENT_RAW        0x00000006
#define NDIS_MEDIUM_ARCENT_878_2    0x00000007
#define NDIS_MEDIUM_ATM            0x00000008
#define NDIS_MEDIUM_WIRELESS_LAN    0x00000009
#define NDIS_MEDIUM_IRDA        0x0000000A
#define NDIS_MEDIUM_BPC            0x0000000B
#define NDIS_MEDIUM_CO_WAN        0x0000000C
#define NDIS_MEDIUM_1394        0x0000000D

#define NDIS_PACKET_TYPE_DIRECTED    0x00000001
#define NDIS_PACKET_TYPE_MULTICAST    0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST    0x00000004
#define NDIS_PACKET_TYPE_BROADCAST    0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING    0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS    0x00000020
#define NDIS_PACKET_TYPE_SMT        0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL    0x00000080
#define NDIS_PACKET_TYPE_GROUP        0x00000100
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL    0x00000200
#define NDIS_PACKET_TYPE_FUNCTIONAL    0x00000400
#define NDIS_PACKET_TYPE_MAC_FRAME    0x00000800

#define NDIS_MEDIA_STATE_CONNECTED    0x00000000
#define NDIS_MEDIA_STATE_DISCONNECTED    0x00000001

#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA     0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED      0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND      0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK             0x00000008
#define NDIS_MAC_OPTION_FULL_DUPLEX             0x00000010
#define NDIS_MAC_OPTION_EOTX_INDICATION         0x00000020
#define NDIS_MAC_OPTION_8021P_PRIORITY          0x00000040
#define NDIS_MAC_OPTION_RESERVED                0x80000000

#endif /* _LINUX_NDIS_H */

/** @} */