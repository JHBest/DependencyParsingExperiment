CC=g++
CFLAGS=-c -Wall -g -rdynamic
LDFLAGS=
SOURCES=src/main.cpp src/DependencyPaser.cpp src/Environment.cpp src/Simulator.cpp src/WordAgent.cpp \
		src/Model.cpp src/Predictor.cpp src/Trainer.cpp src/FeatureTemplate.cpp src/Evaluation.cpp src/Test.cpp \
		src/LocalEnv.cpp src/logger.cpp src/LoggerUtil.cpp src/ParameterValue.cpp src/PredictResult.cpp \
		src/RunParameter.cpp src/SentenceDependency.cpp src/StrHead.cpp src/Tools.cpp src/WordInfo.cpp \
		src/WordSimpleInfor.cpp
OBJECTS=$(patsubst src/%.cpp, obj/%.o, $(SOURCES))
EXECUTABLE=bin/test

$(shell [ -d bin ] || mkdir -p bin)
$(shell [ -d obj ] || mkdir -p obj)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
