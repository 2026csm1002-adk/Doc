# CS509 Repository Makefile
CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall

ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE :=
endif

A1_SRC    := assignment_01/src
A1_DRIVER := assignment_01/driver

BIN_GEMM := $(A1_DRIVER)/driver_gemm$(EXE)
BIN_CSR  := $(A1_DRIVER)/driver_csr$(EXE)

$(BIN_GEMM): $(A1_DRIVER)/driver_gemm.cpp $(A1_SRC)/gemm.cpp $(A1_SRC)/gemm.h
	$(CXX) $(CXXFLAGS) -o $@ $(A1_DRIVER)/driver_gemm.cpp $(A1_SRC)/gemm.cpp

$(BIN_CSR): $(A1_DRIVER)/driver_csr.cpp $(A1_SRC)/csr.cpp $(A1_SRC)/csr.h
	$(CXX) $(CXXFLAGS) -o $@ $(A1_DRIVER)/driver_csr.cpp $(A1_SRC)/csr.cpp

A2_SRC    := assignment_02/src
A2_DRIVER := assignment_02/driver

BIN_BF := $(A2_DRIVER)/driver_bf$(EXE)
BIN_FW := $(A2_DRIVER)/driver_fw$(EXE)

$(BIN_BF): $(A2_DRIVER)/driver_bf.cpp $(A2_SRC)/bellman_ford.cpp $(A2_SRC)/bellman_ford.h $(A1_SRC)/csr.cpp $(A1_SRC)/csr.h
	$(CXX) $(CXXFLAGS) -o $@ $(A2_DRIVER)/driver_bf.cpp $(A2_SRC)/bellman_ford.cpp $(A1_SRC)/csr.cpp

$(BIN_FW): $(A2_DRIVER)/driver_fw.cpp $(A2_SRC)/floyd_warshall.cpp $(A2_SRC)/floyd_warshall.h
	$(CXX) $(CXXFLAGS) -o $@ $(A2_DRIVER)/driver_fw.cpp $(A2_SRC)/floyd_warshall.cpp

A3_SRC    := assignment_03/src
A3_DRIVER := assignment_03/driver

BIN_MST := $(A3_DRIVER)/driver_mst$(EXE)

$(BIN_MST): $(A3_DRIVER)/driver_mst.cpp $(A3_SRC)/mst.cpp $(A3_SRC)/mst.h $(A1_SRC)/csr.cpp $(A1_SRC)/csr.h
	$(CXX) $(CXXFLAGS) -o $@ $(A3_DRIVER)/driver_mst.cpp $(A3_SRC)/mst.cpp $(A1_SRC)/csr.cpp

WRAPPER_DIR := common_wrapper
BIN_WRAPPER := $(WRAPPER_DIR)/wrapper$(EXE)

$(BIN_WRAPPER): $(WRAPPER_DIR)/wrapper.cpp $(A1_SRC)/gemm.cpp $(A1_SRC)/csr.cpp $(A2_SRC)/bellman_ford.cpp $(A2_SRC)/floyd_warshall.cpp $(A3_SRC)/mst.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(WRAPPER_DIR)/wrapper.cpp $(A1_SRC)/gemm.cpp $(A1_SRC)/csr.cpp $(A2_SRC)/bellman_ford.cpp $(A2_SRC)/floyd_warshall.cpp $(A3_SRC)/mst.cpp

TOOLS_DIR := tools
BIN_GEN_GEMM  := $(TOOLS_DIR)/gen_gemm_test$(EXE)
BIN_GEN_GRAPH := $(TOOLS_DIR)/gen_graph$(EXE)
BIN_GEN_BF    := $(TOOLS_DIR)/gen_bf_test$(EXE)
BIN_GEN_FW    := $(TOOLS_DIR)/gen_fw_test$(EXE)
BIN_GEN_MST   := $(TOOLS_DIR)/gen_mst_test$(EXE)

$(BIN_GEN_GEMM): $(TOOLS_DIR)/gen_gemm_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/gen_gemm_test.cpp

$(BIN_GEN_GRAPH): $(TOOLS_DIR)/gen_graph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/gen_graph.cpp

$(BIN_GEN_BF): $(TOOLS_DIR)/gen_bf_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/gen_bf_test.cpp

$(BIN_GEN_FW): $(TOOLS_DIR)/gen_fw_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/gen_fw_test.cpp

$(BIN_GEN_MST): $(TOOLS_DIR)/gen_mst_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLS_DIR)/gen_mst_test.cpp

.PHONY: all gemm csr bf fw mst wrapper tools drivers clean

all: drivers wrapper tools

drivers: gemm csr bf fw mst

gemm: $(BIN_GEMM)
csr:  $(BIN_CSR)
bf:   $(BIN_BF)
fw:   $(BIN_FW)
mst:  $(BIN_MST)

wrapper: $(BIN_WRAPPER)

tools: $(BIN_GEN_GEMM) $(BIN_GEN_GRAPH) $(BIN_GEN_BF) $(BIN_GEN_FW) $(BIN_GEN_MST)

clean:
	rm -f $(BIN_GEMM) $(BIN_CSR) $(BIN_BF) $(BIN_FW) $(BIN_MST) $(BIN_WRAPPER) $(BIN_GEN_GEMM) $(BIN_GEN_GRAPH) $(BIN_GEN_BF) $(BIN_GEN_FW) $(BIN_GEN_MST)
