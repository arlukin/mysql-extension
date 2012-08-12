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

# Where the plugin is installed
libexecdir = /var/lib/mysql/plugins/

# Where the source are stored.
srcdir = src
testdir = test

# Where the build/object/executeable/lib files are stored.
builddir = build

CC = gcc
CFLAGS = -Wall -fPIC -lstdc++ -shared
CTESTFLAGS = -Wall -fPIC -lstdc++ -g

INCLUDE = -I /usr/include/mysql
TESTINCLUDE = -I ./build/unittest-cpp/UnitTest++/src/ -I ./src/
Dflags = -pg

all: $(builddir)/test runtest $(builddir)/folanguage.so

$(builddir)/test: $(testdir)/test.cpp $(testdir)/test_string.cpp $(testdir)/test_data_type_size.cpp $(testdir)/test_language.cpp $(srcdir)/fo_string.h $(srcdir)/udf.hpp $(srcdir)/udf.cpp $(srcdir)/fo_language.hpp $(srcdir)/fo_language.cpp
	gcc -o $(builddir)/test $(CTESTFLAGS) $(TESTINCLUDE) ./build/unittest-cpp/UnitTest++/libUnitTest++.a $(testdir)/test.cpp $(testdir)/test_string.cpp $(testdir)/test_data_type_size.cpp $(testdir)/test_language.cpp $(srcdir)/udf.hpp $(srcdir)/udf.cpp $(srcdir)/fo_language.hpp $(srcdir)/fo_language.cpp

runtest:
	@echo ""
	build/test
	@echo ""

$(builddir)/folanguage.so: $(srcdir)/fo_language.cpp
	gcc -o $(builddir)/folanguage.so $(CFLAGS) $(INCLUDE) $(srcdir)/fo_language.cpp

clean:
	rm $(builddir)/*

install: poke
	cp poke $(libexecdir)/poke
