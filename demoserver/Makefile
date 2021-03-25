PROJECT_NAME := demoserver

all: protoc build

##########################
# Build                  #
##########################

build: ## Build the binary for linux
	CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -ldflags="-X 'github.com/bladedancer/envoy-wasm-stopitertest/demoserver/cmd.BuildTime=$${BUILD_DATE}' \
					-X 'github.com/bladedancer/envoy-wasm-stopitertest/demoserver/cmd.BuildVersion=$${COMMIT_VERSION}' \
					-X 'github.com/bladedancer/envoy-wasm-stopitertest/demoserver/cmd.BuildCommitSha=$${COMMIT_SHORT_SHA}' \
					-X 'github.com/bladedancer/envoy-wasm-stopitertest/demoserver/cmd.BuildName=DemoServer'" \
		-o ./bin/$(PROJECT_NAME) ./main.go

##########################
# Clean                  #
##########################

clean: ## Clean out dir
	rm -rf ./bin

##########################
# Tidy                   #
##########################

tidy: ## Tidy dependencies
	@go mod tidy
	@go mod vendor -v


##########################
# Proto                  #
##########################
PROTODIRS := ../wasm-filter/proto
PROTOFILES := $(shell find $(PROTODIRS) -type f -name '*.proto')
PROTOTARGETS := $(PROTOFILES:.proto=.pb.go)
PROTOALLTARGETS := $(PROTOTARGETS)
PROTOOPTS := -I/go/src/ --go_out=plugins=grpc:/go/src/

%.pb.go: %.proto
	@echo $<
	mkdir -p ./pkg/demoservice
	@protoc --proto_path $(PROTODIRS) -I ./vendor -I . --go_out=Mgoogle/protobuf/any.proto=github.com/golang/protobuf/ptypes/any,plugins=grpc:./pkg/demoservice $<

protoc: $(PROTOALLTARGETS)
