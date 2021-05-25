//****************************************************************************
// SamUsbHost.h
//
// Created 9/9/2020 12:34:32 PM by Tim
//
//****************************************************************************

#pragma once

enum HostPipeType
{
	USB_HOST_PCFG_PTYPE_DISABLE_Val,
	USB_HOST_PCFG_PTYPE_CONTROL_Val,
	USB_HOST_PCFG_PTYPE_ISOCHRONOUS_Val,
	USB_HOST_PCFG_PTYPE_BULK_Val,
	USB_HOST_PCFG_PTYPE_INTERRUPT_Val,
	USB_HOST_PCFG_PTYPE_EXTENDED_Val,
};

enum HostPipeToken
{
	USB_HOST_PCFG_PTOKEN_SETUP_Val,
	USB_HOST_PCFG_PTOKEN_IN_Val,
	USB_HOST_PCFG_PTOKEN_OUT_Val
};

enum PipePacketSize
{
	USB_HOST_PCKSIZE_SIZE_8_Val,
	USB_HOST_PCKSIZE_SIZE_16_Val,
	USB_HOST_PCKSIZE_SIZE_32_Val,
	USB_HOST_PCKSIZE_SIZE_64_Val,
	USB_HOST_PCKSIZE_SIZE_128_Val,
	USB_HOST_PCKSIZE_SIZE_256_Val,
	USB_HOST_PCKSIZE_SIZE_512_Val,
	USB_HOST_PCKSIZE_SIZE_1023_Val
};

#define USB_HOST_PCFG_PTYPE_DISABLE		USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_DISABLE_Val)
#define USB_HOST_PCFG_PTYPE_CONTROL		USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_CONTROL_Val)
#define USB_HOST_PCFG_PTYPE_ISOCHRONOUS	USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_ISOCHRONOUS_Val)
#define USB_HOST_PCFG_PTYPE_BULK		USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_BULK_Val)
#define USB_HOST_PCFG_PTYPE_INTERRUPT	USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_INTERRUPT_Val)
#define USB_HOST_PCFG_PTYPE_EXTENDED	USB_HOST_PCFG_PTYPE(USB_HOST_PCFG_PTYPE_EXTENDED_Val)

#define USB_HOST_PCFG_PTOKEN_SETUP		USB_HOST_PCFG_PTOKEN(USB_HOST_PCFG_PTOKEN_SETUP_Val)
#define USB_HOST_PCFG_PTOKEN_IN			USB_HOST_PCFG_PTOKEN(USB_HOST_PCFG_PTOKEN_IN_Val)
#define USB_HOST_PCFG_PTOKEN_OUT		USB_HOST_PCFG_PTOKEN(USB_HOST_PCFG_PTOKEN_OUT_Val)

#define USB_HOST_PCKSIZE_SIZE_8			USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_8_Val)
#define USB_HOST_PCKSIZE_SIZE_16		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_16_Val)
#define USB_HOST_PCKSIZE_SIZE_32		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_32_Val)
#define USB_HOST_PCKSIZE_SIZE_64		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_64_Val)
#define USB_HOST_PCKSIZE_SIZE_128		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_128_Val)
#define USB_HOST_PCKSIZE_SIZE_256		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_256_Val)
#define USB_HOST_PCKSIZE_SIZE_512		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_512_Val)
#define USB_HOST_PCKSIZE_SIZE_1023		USB_HOST_PCKSIZE_SIZE(USB_HOST_PCKSIZE_SIZE_1023_Val)