#
# Copyright (C) 2011-2021 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

include ../buildenv.mk

App_Name := ehsm-dkeycache
Enclave_Name := libenclave-$(App_Name).so
Signed_Enclave_Name := libenclave-$(App_Name).signed.so

Enclave_Config_File := Enclave/enclave.config.xml
Enclave_Signing_Key := Enclave/enclave_private_test.pem

OUT = $(TOPDIR)/$(OUTDIR)/$(App_Name)

######## App Settings ########

App_Cpp_Files := $(wildcard App/*.cpp)

App_Include_Paths := \
	-IApp \
	-I$(SGX_SDK)/include \
	-I$(TOPDIR)/include

App_C_Flags := $(SGX_COMMON_FLAGS) -fPIC -Wno-attributes $(App_Include_Paths)

# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
    App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
    App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
    App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cpp_Flags := $(App_C_Flags) -std=c++14
App_Link_Flags := -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -lpthread -lra_ukey_exchange -L$(TOPDIR)/$(OUTLIB_DIR) -Wl,-rpath=$(CURDIR)

ifneq ($(SGX_MODE), HW)
    App_Link_Flags += -lsgx_epid_sim -lsgx_quote_ex_sim
else
    App_Link_Flags += -lsgx_quote_ex -lsgx_uae_service -lsgx_dcap_ql -ldl
endif

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)


######## Enclave Settings ########
Enclave_Cpp_Files := $(wildcard Enclave/*.cpp)

Enclave_Include_Paths := \
	-IEnclave \
	-I$(SGX_SDK)/include \
	-I$(SGX_SDK)/include/tlibc \
	-I$(SGX_SDK)/include/libcxx \
	-I$(TOPDIR)/include

Enclave_C_Flags := \
	$(Enclave_Include_Paths) \
	-nostdinc -fvisibility=hidden \
	-fpie -ffunction-sections \
	-fdata-sections \
	$(MITIGATION_CFLAGS)

CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
	Enclave_C_Flags += -fstack-protector
else
	Enclave_C_Flags += -fstack-protector-strong
endif

Enclave_Cpp_Flags := $(Enclave_C_Flags) -nostdinc++

# Enable the security flags
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
Enclave_Link_Flags := $(MITIGATION_LDFLAGS) $(Enclave_Security_Link_Flags) \
    -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_TRUSTED_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -lsgx_tcrypto -l$(Service_Library_Name) -lra_tkey_exchange -L$(TOPDIR)/$(OUTLIB_DIR) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave/enclave.lds

Enclave_Cpp_Objects := $(sort $(Enclave_Cpp_Files:.cpp=.o))

######## Build Settings ########
.PHONY: all target clean
all: target
	@$(MAKE) target
	@mkdir -p $(OUT)
	@mv $(App_Name) $(Signed_Enclave_Name) $(Enclave_Name) $(OUT)

ifeq ($(Build_Mode), HW_RELEASE)
target: $(App_Name) $(Enclave_Name)
else
target: $(App_Name) $(Signed_Enclave_Name)
endif

clean:
	@rm -f $(App_Cpp_Objects) $(Enclave_Cpp_Objects)  App/auto_version.h App/enclave_u.* Enclave/enclave_t.*
	@rm -rf $(OUT)

######## App Objects ########
App/auto_version.h:
	@`touch App/auto_version.h`
	@echo "#ifndef EHSM_DKEYCACHE_AUTO_VERSION_H" > App/auto_version.h
	@echo "#define EHSM_DKEYCACHE_AUTO_VERSION_H" >> App/auto_version.h
	@echo "#define EHSM_VERSION \"$(VERSION_STRING)\"" >> App/auto_version.h
	@echo "#define EHSM_DATE \"$(DATE_STRING)\""	>> App/auto_version.h
	@echo "#define EHSM_GIT_SHA \"$(EHSM_KMS_GIT_SHA)\""	>> App/auto_version.h
	@echo "#endif //EHSM_DKEYCACHE_AUTO_VERSION_H"	>> App/auto_version.h
	@echo "  ==>  touch App/auto_version.h"

App/enclave_u.h: $(SGX_EDGER8R) Enclave/enclave.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave/enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/enclave_u.c: App/enclave_u.h

App/enclave_u.o: App/enclave_u.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp App/auto_version.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/enclave_u.o $(App_Cpp_Objects) App/auto_version.h
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"

######## Enclave Objects ########

Enclave/enclave_t.h: $(SGX_EDGER8R) Enclave/enclave.edl
	@cd Enclave && $(SGX_EDGER8R) --trusted ../Enclave/enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave/enclave_t.c: Enclave/enclave_t.h

Enclave/enclave_t.o: Enclave/enclave_t.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave/%.o: Enclave/%.cpp Enclave/enclave_t.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(Enclave_Name): Enclave/enclave_t.o $(Enclave_Cpp_Objects)
	@$(CXX) $^ -o $@ $(Enclave_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_Name): $(Enclave_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key $(Enclave_Signing_Key) -enclave $(Enclave_Name) -out $@ -config $(Enclave_Config_File)
	@echo "SIGN =>  $@"
