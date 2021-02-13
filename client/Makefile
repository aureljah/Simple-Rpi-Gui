
RM = rm -rf

CP = cp -v

CMAKE = cmake

BUILD_DIR = build

DEBUG_DIR = debug

PATH_TO_CERT = mycert.pem

all: build

build:
	mkdir $(BUILD_DIR)
	$(CMAKE) . -B./$(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	$(CP) $(PATH_TO_CERT) $(BUILD_DIR)

debug:
	mkdir $(DEBUG_DIR)
	$(CMAKE) . -B./$(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug
	$(CP) $(PATH_TO_CERT) $(DEBUG_DIR)

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(DEBUG_DIR)

fclean: clean

re: fclean all

.PHONY: all re clean fclean
