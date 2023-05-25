BASIC_VERT:=$(bd)/basic_vert.spv
BASIC_FRAG:=$(bd)/basic_frag.spv

TGTS_$(d) 	:=$(BASIC_VERT) $(BASIC_FRAG)

TGT_ETC		:=$(TGT_ETC) $(TGTS_$(d))

$(BASIC_VERT): $(d)/basic.vert
	glslc $^ -o $@

$(BASIC_FRAG): $(d)/basic.frag
	glslc $^ -o $@