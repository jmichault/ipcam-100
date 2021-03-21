bin : plenumebloj por T21.

fontoj : fontoj de bibliotekoj kaj programoj.
	
include : include-dosierojn de bibliotekoj kaj T20-SDK.
* T20-SDK (imp sysutils)
* live555 (BasicUsageEnvironment  groupsock liveMedia UsageEnvironment)
* openssl
* pcre

lib : bibliotekoj
* live555 (libBasicUsageEnvironment, libliveMedia, libgroupsock, libUsageEnvironment)
* T21 (libalog.so, libimp.so, libsysutils.so)
* openssl
* pcre

toolchain : krucaj kompililoj. por debian buster x64 al mips.
* gcc
* ld
* ...
