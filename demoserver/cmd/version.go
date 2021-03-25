package cmd

import (
	"fmt"

	"github.com/spf13/cobra"
)

// BuildTime -
var BuildTime string

// BuildVersion -
var BuildVersion string

// BuildCommitSha -
var BuildCommitSha string

// BuildName -
var BuildName string

func init() {
	rootCmd.AddCommand(versionCmd)
}

var versionCmd = &cobra.Command{
	Use:   "version",
	Short: "Version information",
	Long:  `Print the version number of Server`,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Printf("%s v%s (%s-%s)\n", BuildName, BuildVersion, BuildTime, BuildCommitSha)
	},
}
