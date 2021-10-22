#include "gameboy.h"
namespace TKPEmu::Gameboy {
	Gameboy::Gameboy() : cpu_(&bus_), ppu_(&bus_), cartridge_() {

	}
	void Gameboy::Reset() {
		cpu_.Reset();
		ppu_.Reset();
	}
	void Gameboy::Update() {
		cpu_.Update();
		//ppu_.Update();
		//if (ppu_.NeedsDraw()) {
		//	ScreenDataMutex.lock();
		//	ppu_.Draw(EmulatorImage);
		//	ScreenDataMutex.unlock();
		//}
	}
	void Gameboy::LoadFromFile(const std::string& path) {
		cartridge_.Load(path, bus_.mem);
	}
	// TODO: LoadInstrToVec only works for rom_only for now
	// TODO: if you win + D while in disassembler, game crashes
	void Gameboy::LoadInstrToVec(std::vector<DisInstr>& vec, bool& finished) {
		auto func = [this, &finished](std::vector<DisInstr>& vec) {
			for (uint16_t i = 0; i < 0x8000;) {
				uint8_t ins = cpu_.bus_->Read(i);
				auto x = cpu_.instructions[ins];
				auto d = DisInstr(i, ins);
				uint8_t p1, p2;
				if (x.skip == 1) {
					p1 = cpu_.bus_->Read(i + 1);
					d.Params[0] = p1;
					std::stringstream ss;
					ss << x.name << " " << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)p1;
					d.InstructionFull = ss.str();
					std::stringstream ss2;
					ss2 << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)ins << " " << std::setfill('0') << std::setw(2) << (int)p1;
					d.InstructionHex = ss2.str();
					vec.push_back(std::move(d));
				}
				else if (x.skip == 2) {
					p1 = cpu_.bus_->Read(i + 1);
					p2 = cpu_.bus_->Read(i + 2);
					d.Params[0] = p1;
					d.Params[1] = p2;
					std::stringstream ss;
					ss << x.name << " " << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)p2 << std::setfill('0') << std::setw(2) << (int)p1;
					d.InstructionFull = ss.str();
					std::stringstream ss2;
					ss2 << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)ins << " " << std::setfill('0') << std::setw(2) << (int)p1 << " " << std::setfill('0') << std::setw(2) << (int)p2;
					d.InstructionHex = ss2.str();
					vec.push_back(std::move(d));
				}
				else {
					std::stringstream ss;
					ss << x.name;
					d.InstructionFull = ss.str();
					vec.push_back(std::move(d));
				}
				i += 1 + x.skip;
			}
			finished = true;
		};
		std::thread t1(func, std::ref(vec));
		t1.detach();
	}
}