package demoservice

import (
	"fmt"
	"net"

	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

type DemoServer struct {
	Config *DemoServiceConfig
	done   chan struct{}
}

func (ps *DemoServer) Run() error {
	ps.done = make(chan struct{})
	ps.runGRPC()
	select {
	case <-ps.done:
	}
	return nil
}

func (ps *DemoServer) runGRPC() (*grpc.Server, error) {
	bind := fmt.Sprintf("%s:%d", ps.Config.Host, ps.Config.Port)
	grpcServer := grpc.NewServer()
	reflection.Register(grpcServer)

	lis, err := net.Listen("tcp", bind)
	if err != nil {
		return nil, err
	}

	RegisterDemoServerServer(grpcServer, ps)

	go func() {
		if err = grpcServer.Serve(lis); err != nil {
			log.Fatal(err)
		}
		ps.done <- struct{}{}
	}()

	log.Infof("GRPC Listening on %s", bind)

	return grpcServer, nil
}
