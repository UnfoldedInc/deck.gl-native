#include <iostream>
#include "json/json.h"

void hello() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["hello"] = "world";
    writer->write(root, &std::cout);
}
