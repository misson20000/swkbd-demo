TARGET := example
OBJECTS := example.o ILibraryAppletCreator.o ILibraryAppletAccessor.o IStorage.o IStorageAccessor.o

all: $(TARGET).nro $(TARGET).nso

clean:
	rm -f *.o *.nro *.nso *.so

# include libtransistor rules
ifndef LIBTRANSISTOR_HOME
    $(error LIBTRANSISTOR_HOME must be set)
endif
include $(LIBTRANSISTOR_HOME)/libtransistor.mk

$(TARGET).nro.so: $(OBJECTS) $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	$(LD) $(LD_FLAGS) -o $@ $(OBJECTS) $(LIBTRANSISTOR_NRO_LDFLAGS)

$(TARGET).nso.so: $(OBJECTS) $(LIBTRANSITOR_NSO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	$(LD) $(LD_FLAGS) -o $@ $(OBJECTS) $(LIBTRANSISTOR_NSO_LDFLAGS)
