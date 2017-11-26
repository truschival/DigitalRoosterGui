# DigitalRooster

Internet radio and podcast player


## Possible BUILD_TYPE(s):
- Debug
- Release (default
- RelWithDebInfo

## Options & Defaults (compilation flags & targets):
- -DBUILD_TEST=OFF      (build google test)
                        requires GTest installed as library or `-DBUILD_GTEST_FROM_SRC=On`
- -DBUILD_GTEST_FROM_SRC=Off (recompile gtest as external project)
- -DTEST_COVERAGE=Off   (code coverage)
