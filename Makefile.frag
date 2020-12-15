ext-subtree:
	./scripts/subtree.sh

ext-fetch:
	./scripts/fetch.php

ext-prepare:
	echo "prepare..."
	phpize && ./configure --enable-debug

ext-clean:
	echo "clean..."
	make clean && phpize --clean

ext-build:
	./scripts/build.sh

ext-test:
	# echo "disable ext startup.so"
	phpbrew ext disable startup
	MINBABY_TEST_EXT=0 php ./vendor/bin/kahlan

ext-test-ext:
	# echo "enable ext start.so"
	phpbrew ext enable startup
	MINBABY_TEST_EXT=1 php ./vendor/bin/kahlan
	
ext-first:ext-prepare ext-build ext-test ext-test-ext
	@echo Done
