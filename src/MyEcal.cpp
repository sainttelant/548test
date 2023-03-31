
#include "MyEcal.h"
#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>

eCAL::string::CPublisher<string> txt_pub_;




void init_ecal(int argc, char** argv) {
	eCAL::Initialize(argc, argv, "radar540");
	txt_pub_.Create("Txt079");
}

void pub_ecal_msg(const string& msg) {
	txt_pub_.Send(msg);
}


