#include "DevDevice.h"

using namespace FicsItKernel;
using namespace FicsItKernel::FicsItFS;

DevDevice::DevDevice() {
	stdio = new FileSystem::MemFile(FileSystem::ListenerListRef(listeners, ""), nullptr);
}

FileSystem::SRef<FileSystem::FileStream> DevDevice::open(FileSystem::Path path, FileSystem::FileMode mode) {
	path.absolute = false;
	if (path == "stdio") {
		return stdio->open(mode);
	}
	return nullptr;
}

FileSystem::SRef<FileSystem::Node> DevDevice::get(FileSystem::Path path) {
	try {
		return new FileSystem::DeviceNode(devices.at(path.str()));
	} catch (...) {
		return nullptr;
	}
}

bool DevDevice::remove(FileSystem::Path path, bool recursive) {
	return false;
}

FileSystem::SRef<FileSystem::Directory> DevDevice::createDir(FileSystem::Path, bool tree) {
	return nullptr;
}

bool DevDevice::rename(FileSystem::Path path, const FileSystem::NodeName& name) {
	return false;
}

std::unordered_set<FileSystem::NodeName> DevDevice::childs(FileSystem::Path path) {
	std::unordered_set<FileSystem::NodeName> list;
	for (auto device : devices) {
		list.insert(device.first);
	}
	return list;
}

bool DevDevice::addDevice(FileSystem::SRef<FileSystem::Device> device, const FileSystem::NodeName& name) {
	auto dev = devices.find(name);
	if (dev != devices.end()) return false;
	devices[name] = device;
	return true;
}

bool DevDevice::removeDevice(FileSystem::SRef<FileSystem::Device> device) {
	for (auto d = devices.begin(); d != devices.end(); d++) {
		if (d->second == device) {
			devices.erase(d);
			return true;
		}
	}
	return false;
}

std::unordered_map<FileSystem::NodeName, FileSystem::SRef<FileSystem::Device>> FicsItKernel::FicsItFS::DevDevice::getDevices() {
	return devices;
}

void DevDevice::updateCapacity(std::int64_t capacity) {
	for (auto& device : devices) {
		if (FileSystem::MemDevice* memDev = dynamic_cast<FileSystem::MemDevice*>(device.second.get())) {
			memDev->capacity = memDev->getUsed() + capacity;
		}
	}
}

void DevDevice::tickListeners() {
	for (auto& device : devices) {
		if (FileSystem::DiskDevice* diskDev = dynamic_cast<FileSystem::DiskDevice*>(device.second.get())) {
			diskDev->tickWatcher();
		}
	}
}