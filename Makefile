default:
	cd services/manager && npm run build
	cd platform/desktop && $(MAKE)

.PHONY: clean
clean:
	cd platform/desktop && $(MAKE) clean
