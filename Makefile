
#The Target Binary Program
TARGET      := FSA
CC          := g++

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o


#Flags, Libraries and Includes
CFLAGS      := -std=c++0x -pthread -O0 -g -Wall -fmessage-length=0 -rdynamic  -fpermissive 
LIB         := -ldl  
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)


## Compile Sensitivity models
SenModels:
	$(CC) -O2  -std=c++0x -fPIC -rdynamic -shared  res/SenModels/HwSM.cpp -o res/SenModels/HwSM.so
	$(CC) -O2  -std=c++0x -fPIC -rdynamic -shared  res/SenModels/NonZeroSM.cpp -o res/SenModels/NonZeroSM.so
	$(CC) -O2  -std=c++0x -fPIC -rdynamic -shared  res/SenModels/MaximumSMT.cpp -o res/SenModels/MaximumSMT.so
	$(CC) -O2  -std=c++0x -fPIC -rdynamic -shared  res/SenModels/MeanSMT.cpp -o res/SenModels/MeanSMT.so
	$(CC) -O2  -std=c++0x -fPIC -rdynamic -shared  res/SenModels/MinimumSMT.cpp -o res/SenModels/MinimumSMT.so


#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))


#Defauilt Make
all: SenModels resources $(TARGET)


#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories
	@cp -r "$(RESDIR)/". $(TARGETDIR)/ 

#Make the Directories
directories:  
	echo $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
cleanObj: 
	@$(RM) -rf $(BUILDDIR)/*

#Full Clean, Objects and Binaries
clean: cleanObj
	@$(RM) -rf $(TARGETDIR)/*

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p obj
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

docs:
	mkdir -p Doc
	doxygen Doxyfile 
	
docs_clean:
	@rm -f Doc

	
#Non-File Targets
.PHONY: all remake clean cleaner resources



