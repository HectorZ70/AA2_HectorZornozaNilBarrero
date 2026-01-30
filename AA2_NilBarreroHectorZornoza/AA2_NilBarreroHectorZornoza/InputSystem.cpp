#include "InputSystem.h"
#include "ConsoleControl_.h"

InputSystem::KeyBinding::KeyBinding(int key, OnKeyPress onKeyPress)
{
	_key = key;
	_onKeyPress = onKeyPress;
}

InputSystem::KeyBinding::~KeyBinding()
{

}

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	for (std::pair<int, KeyBindingList> pair : _keyBindingMap)
	{
		KeyBindingList bindinsList = pair.second;
		for (KeyBinding* binding : bindinsList)
		{
			delete binding;
		}

		bindinsList.clear();
	}
	_keyBindingMap.clear();
}

InputSystem::KeyBinding* InputSystem::AddListener(int key, KeyBinding::OnKeyPress onKeyPress)
{
	_classMutex.lock();

	if (_keyBindingMap.find(key) == _keyBindingMap.end())
	{
		_keyBindingMap[key] = KeyBindingList();
	}

	KeyBinding* keyBinding = new KeyBinding(key, onKeyPress);
	_keyBindingMap[key].push_back(keyBinding);

	_classMutex.unlock();

	return keyBinding;
}

void InputSystem::RemoveAndDeleteListener(KeyBinding* keyBinding)
{
	int key = keyBinding->_key;
	_classMutex.lock();

	if (_keyBindingMap.find(key) != _keyBindingMap.end())
	{
		KeyBindingList list = _keyBindingMap[key];
		list.remove(keyBinding);
		delete keyBinding;

		if (list.size() <= 0)
		{
			_keyBindingMap.erase(key);
		}
	}

	_classMutex.unlock();
}

void InputSystem::StartListen()
{
	_classMutex.lock();

	if (_state != Stopped)
	{
		_classMutex.unlock();
		return;
	}

	_state = Starting;

	std::thread* listenLoopThread = new std::thread(&InputSystem::ListenLoop, this);
	listenLoopThread->detach();

	_classMutex.unlock();
}

void InputSystem::StopListen()
{
	_classMutex.lock();
	if (_state != Listening)
	{
		_classMutex.unlock();
		return;
	}

	_state = Stopping;

	_classMutex.unlock();
}

void InputSystem::ListenLoop()
{
	{
		std::lock_guard<std::mutex> lock(_classMutex);
		_state = Listening;
		CC::ClearKeyBuffer();
	}

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(_classMutex);
			if (_state != Listening)
				break;
		}

		int key = CC::ReadNextKey(); 

		if (key != 0)
		{
			std::lock_guard<std::mutex> lock(_classMutex);

			auto it = _keyBindingMap.find(key);
			if (it != _keyBindingMap.end())
			{
				for (KeyBinding* binding : it->second)
				{
					binding->_onKeyPress();
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	std::lock_guard<std::mutex> lock(_classMutex);
	_state = Stopped;
}
