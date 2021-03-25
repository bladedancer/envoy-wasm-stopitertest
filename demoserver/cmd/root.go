package cmd

import (
	"strings"

	"github.com/bladedancer/envoy-wasm-stopitertest/demoserver/pkg/demoservice"
	"github.com/bladedancer/envoy-wasm-stopitertest/demoserver/pkg/log"
	"github.com/spf13/cobra"
	"github.com/spf13/pflag"
	"github.com/spf13/viper"
)

var rootCmd = &cobra.Command{
	Use:   "demoserver",
	Short: "Demo Server.",
	Long:  `Implementation of the Demo GRPC interface.`,
	RunE:  run,
}

// Execute executes the root command.
func Execute() error {
	return rootCmd.Execute()
}

func init() {
	cobra.OnInitialize(initRootConfig)

	rootCmd.PersistentFlags().String("logLevel", "info", "log level")
	rootCmd.PersistentFlags().String("logFormat", "json", "line or json")
	bindOrPanic("log.level", rootCmd.PersistentFlags().Lookup("logLevel"))
	bindOrPanic("log.format", rootCmd.PersistentFlags().Lookup("logFormat"))

	rootCmd.PersistentFlags().String("host", "0.0.0.0", "host interface to bind to")
	rootCmd.PersistentFlags().Uint32("port", 50051, "port to listen on")
	bindOrPanic("host", rootCmd.PersistentFlags().Lookup("host"))
	bindOrPanic("port", rootCmd.PersistentFlags().Lookup("port"))

}

func initRootConfig() {
	viper.SetTypeByDefaultValue(true)
	viper.SetEnvPrefix("pe")
	viper.SetEnvKeyReplacer(strings.NewReplacer(".", "_"))
	viper.AutomaticEnv()

	err := log.SetLogLevel(viper.GetString("log.level"), viper.GetString("log.format"))
	if err != nil {
		panic(err)
	}
}

func bindOrPanic(key string, flag *pflag.Flag) {
	if err := viper.BindPFlag(key, flag); err != nil {
		panic(err)
	}
}

func run(cmd *cobra.Command, args []string) error {
	// Create the file source and run the server command
	ps := &demoservice.DemoServer{
		Config: &demoservice.DemoServiceConfig{
			Host: viper.GetString("host"),
			Port: viper.GetUint32("port"),
		},
	}

	return ps.Run()
}
