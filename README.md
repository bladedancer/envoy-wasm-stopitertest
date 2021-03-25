# Build
Run the `./buildall.sh` it will build the wasm filter and the grpc backend server.

# Run
Start the grpc server:
`./bin/demoserver --port 50051`

Start envoy:
`getenvoy run standard:1.17.1 -- -l trace --config-path ./envoy/envoy-local.yaml`