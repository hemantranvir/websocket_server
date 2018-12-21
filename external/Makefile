POCO_TGZ = poco-1.9.0-all.tar.gz
POCO_DIR = poco-1.9.0-all
POCO_INSTALL_DIR = poco

all: poco-build

poco-build: poco_extract
	$(if $(shell ls $(POCO_INSTALL_DIR)/lib/libPocoNet.so), @echo "already built poco.", \
	$(shell cd $(POCO_INSTALL_DIR)/$(POCO_DIR) && ./configure --prefix=../../$(POCO_INSTALL_DIR) --config=Linux --omit=NetSSL_OpenSSL,Crypto,Data/ODBC,Data/MySQL,Redis,MongoDB,PageCompiler,Zip,Data --no-tests --no-samples && make install))

poco_extract:
	$(if $(shell ls $(POCO_INSTALL_DIR)), , mkdir $(POCO_INSTALL_DIR))
	$(if $(shell ls $(POCO_INSTALL_DIR)), , tar zxf $(POCO_TGZ) -C $(POCO_INSTALL_DIR))
