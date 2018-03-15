DigitalRooster
===================

Internet radio and podcast player


-----
# Build configuration

## Options & Defaults (compilation flags & targets):
- -DBUILD_TEST=OFF      (build google test)
                        requires GTest installed as library or `-DBUILD_GTEST_FROM_SRC=On`
- -DBUILD_GTEST_FROM_SRC=Off (recompile gtest as external project)
- -DTEST_COVERAGE=Off   (code coverage)


## Build Setup
	
	mkdir build
	
	cd build



### Debug & coverage configuration	

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 -DBUILD_TEST=On -DTEST_COVERAGE=On -DBUILD_GTEST_FROM_SRC=On <this_directory>

### Release

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 <this_directory>

###Build command
	
	cmake --build .  --target all
	
of just 
	
	make -j 4
	
### Create Doxygen documentation (if Doxygen is installed)

	make DOC
	
-------






