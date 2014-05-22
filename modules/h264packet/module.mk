#
# module.mk
#
# Copyright (C) 2010 Creytiv.com
#
 

MOD		:= h264packet
$(MOD)_SRCS	+= h264packet.c h264.c encode.c decode.c

include mk/mod.mk
