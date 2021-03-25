package log

import (
	"errors"
	"time"

	"github.com/sirupsen/logrus"
)

// RootLogger root logger
var logger *logrus.Logger = logrus.New()

// Logger -
type Logger interface {
	Fatal(args ...interface{})
	Fatalf(fmt string, args ...interface{})
	Error(args ...interface{})
	Errorf(fmt string, args ...interface{})
	Warn(args ...interface{})
	Warnf(fmt string, args ...interface{})
	Info(args ...interface{})
	Infof(fmt string, args ...interface{})
	Debug(args ...interface{})
	Debugf(fmt string, args ...interface{})
}

const (
	lineFormat = "line"
	jsonFormat = "json"
)

func getFormatter(format string) (logrus.Formatter, error) {
	switch format {
	case lineFormat:
		return &logrus.TextFormatter{TimestampFormat: time.RFC3339}, nil
	case jsonFormat:
		return &logrus.JSONFormatter{TimestampFormat: time.RFC3339}, nil
	default:
		return nil, errors.New("invalid log format")
	}
}

// SetLogLevel configures the root logger
func SetLogLevel(level string, format string) error {
	lvl, err := logrus.ParseLevel(level)
	if err != nil {
		return err
	}

	formatter, err := getFormatter(format)
	if err != nil {
		return err
	}

	logger.Level = lvl
	logger.Formatter = formatter

	return nil
}

// WithPackage returns a logger with the package field.
func WithPackage(name string) Logger {
	return logger.WithField("package", name)
}

// WithField returns a logger with the field.
func WithField(key string, value interface{}) Logger {
	return logger.WithField(key, value)
}
