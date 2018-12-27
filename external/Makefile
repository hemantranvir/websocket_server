GTEST_TGZ = googletest-release-1.8.0.tar.gz
GTEST_EXTRACT_DIR = googletest-release-1.8.0
GTEST_ARM_INSTALL_DIR = gtest-1.8.0-arm
GTEST_INSTALL_DIR = gtest-1.8.0

POCO_TGZ = poco-1.9.0-all.tar.gz
POCO_DIR = poco-1.9.0-all
POCO_INSTALL_DIR = poco

all: gtest-build poco-build

gtest-build: gtest_extract
	$(if $(shell ls $(GTEST_INSTALL_DIR)/lib/libgtest.a), @echo "already built gtest.", \
	mkdir -p $(GTEST_INSTALL_DIR)/build && cd $(GTEST_INSTALL_DIR)/build && cmake ../../$(GTEST_EXTRACT_DIR) -DCMAKE_INSTALL_PREFIX=.. -DBUILD_GMOCK:BOOL=ON -DBUILD_GTEST:BOOL=ON && make install)

gtest_extract:
	$(if $(shell ls $(GTEST_EXTRACT_DIR)), , tar -xzf $(GTEST_TGZ))

poco-build: poco_extract
	$(if $(shell ls $(POCO_INSTALL_DIR)/lib/libPocoNet.so), @echo "already built poco.", \
	$(shell cd $(POCO_INSTALL_DIR)/$(POCO_DIR) && ./configure --prefix=../../$(POCO_INSTALL_DIR) --config=Linux --omit=NetSSL_OpenSSL,Crypto,Data/ODBC,Data/MySQL,Redis,MongoDB,PageCompiler,Zip,Data --no-tests --no-samples && make install))

poco_extract:
	$(if $(shell ls $(POCO_INSTALL_DIR)), , mkdir $(POCO_INSTALL_DIR))
	$(if $(shell ls $(POCO_INSTALL_DIR)), , tar zxf $(POCO_TGZ) -C $(POCO_INSTALL_DIR))
