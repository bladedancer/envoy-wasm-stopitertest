package demoservice

import (
	"fmt"
	"io"
)

func (ps *DemoServer) InvokeDemo(stream DemoServer_InvokeDemoServer) error {
	resp := &DemoReply{}
	first := true

	for {
		req, err := stream.Recv()
		if err == io.EOF {
			log.Info("Premature EOF")
			return fmt.Errorf("Premature EOF")
		}
		if err != nil {
			return err
		}

		log.Infof("Got request: %v", req)

		// The headers are on the first request only.
		if first {
			log.Info("Setting headers")
			policyHeaders := map[string]string{
				"x-added": "new key",
			}
			for key, value := range req.Headers {
				policyHeaders[key] = value
			}
			resp.Headers = policyHeaders
			resp.Id = req.Id
		}

		if len(req.Body) > 0 {
			// SHOULD DO BUFFERING, CHUNKING AND WHAT NOT
			log.Info("Setting body")
			resp.Body = reverseBody(req.GetBody())
		}

		if req.EndOfStream {
			log.Info("Request finished")
			break
		}
		first = false
	}

	// Send the response
	resp.EndOfStream = true
	log.Infof("Responding %v", resp)
	return stream.Send(resp)
}

func reverseBody(input []byte) []byte {
	// -2 to preseve tailing null
	for i, j := 0, len(input)-2; i < j; i, j = i+1, j-1 {
		input[i], input[j] = input[j], input[i]
	}
	return input
}
