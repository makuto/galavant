===============================
thirdPartyWrapper
===============================

Because I'm insane and paranoid, all third party code must be wrapped by a thin defined here in thirdPartyWrapper.

I want to make it easy to swap entire libraries without having to modify any code in Galavant. This wrapper layer facilitates that.

At final link time for Galavant, the only library that should need to be specified should be thirdPartyWrapper.a.