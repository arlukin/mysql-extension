# Read more.
# http://mrbook.org/tutorials/make/
# http://stackoverflow.com/questions/2374863/makefile-build-directory-and-dependencies-list
# http://www.gnu.org/prep/standards/html_node/Directory-Variables.html
# http://airs.com/ian/configure/configure_2.html
# http://www.gnu.org/software/make/manual/make.html
# http://mrbook.org/tutorials/make/

# TODO
# * Add valgrind
# * Add the compiler marfre talked about
# * Run unittests before installing into mysql
# * Run "integration-tests" when installed into mysql
# * Download unittest-cpp and place in build folder.

# Where the plugin is installed @todo
libexecdir = /var/lib/mysql/plugins/

# Where the build/object/executeable/lib files are stored.
BUILDDIR = ./build/

# The output file generated by the make file.
lib = $(BUILDDIR)foLanguage.so

# Executeable unit test.
test = $(BUILDDIR)testFoLanguage

# Include pathes where compiler looks for header files and other includes.
INCLUDE = -I./ -I./src/ -I./build/UnitTest++/src/ -I/usr/include/mysql/

# All files that should be compiled to generate 'lib'
src = src/fo_debug.cpp \
      src/fo_language.cpp \
      src/fo_string.cpp \
      src/udf.cpp

# All files that should be compiled to generate 'test'
test_src = src/test/test.cpp \
 	src/test/test_string.cpp \
 	src/test/test_data_type_size.cpp \
 	src/test/test_language.cpp

# Compiler flags, and path to different used compile software.
#
CXX = g++
CXXDEBUG = -g
CXXFLAGS ?= $(CXXDEBUG) -Wall -W -ansi $(INCLUDE) # -pedantic
LDFLAGS ?= -L./build/UnitTest++/ -lUnitTest++
SED = sed
MV = mv
RM = rm

#CFLAGS = -shared -lstdc++

#
objects = $(patsubst %.cpp, %.o, $(src))
test_objects = $(patsubst %.cpp, %.o, $(test_src))
dependencies = $(subst .o,.d,$(objects))
test_dependencies = $(subst .o,.d,$(test_objects))

.SUFFIXES: .o .cpp

define make-depend
  $(CXX) $(CXXFLAGS) -M $1 | \
  $(SED) -e 's,\($(notdir $2)\) *:,$(dir $2)\1: ,' > $3.tmp
  $(SED) -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$//' \
      -e '/^$$/ d' \
      -e 's/$$/ :/' $3.tmp >> $3.tmp
  $(MV) $3.tmp $3
endef


all: $(test)

# Build 'lib'
$(lib): $(objects)
	@echo
	@echo Creating $(lib) library...
	@ar cr $(lib) $(objects)
	@echo

# Build 'test'
$(test): $(lib) $(test_objects)
	@echo
	@echo Linking $(test)...
	@$(CXX) -o $(test) $(test_objects) $(lib) $(LDFLAGS)
	@echo
	echo Running unit tests...
	@./$(test)

	valgrind --tool=memcheck ./$(test)

	#mysql -uroot -p foTest < ./test/test.sql

	#tail /var/log/mysqld.log


clean:
	-@$(RM) $(objects) $(test_objects) $(dependencies) $(test_dependencies) $(test) $(lib) 2> /dev/null

# Compile all cpp files into object files.
%.o : %.cpp
	@echo Compile $<
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CXX) $(CXXFLAGS) -c $< -o $(patsubst %.cpp, %.o, $<)

install:
	sudo /etc/init.d/mysqld stop
	cp ../Bin/FoFunctions.so /var/lib/mysql/plugins/FoFunctions.so
	cp ../Bin/FoLanguage.so  /var/lib/mysql/plugins/FoLanguage.so
	sudo /etc/init.d/mysqld start

	#mysql -uroot -p
	#use mysql;

	#DROP function getLanguage;
	#DROP function setLanguage;

	#CREATE FUNCTION getLanguage RETURNS STRING SONAME "FoLanguage.so";
	#CREATE FUNCTION setLanguage RETURNS STRING SONAME "FoLanguage.so";

requirments:
	yum -y install mysql-devel
	yum -y install gcc-c++

# # Not legal make config
# unittet: UnitTest-install
# 	mkdir build
# 	cd build
# 	wget http://sourceforge.net/projects/files/latest/download?source=files
# 	yum -y install unzip
# 	unzip unittest-cpp-1.4.zip
# 	rm -f unittest-cpp-1.4.zip
# 	cd UnitTest++/
# 	make
