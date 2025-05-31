#pragma once

#define FS_WARN_BEG() __pragma(warning(push))

#define FS_WARN_END() __pragma(warning(pop))

#define FS_WARN_WCONV() __pragma(warning(disable : 4244))