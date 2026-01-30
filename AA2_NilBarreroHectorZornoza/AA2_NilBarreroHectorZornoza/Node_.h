#pragma once

#include <mutex>

#include "Vector2_.h"
#include "INodeContent.h"

class Node
{
public:
	Node(Vector2 position);

	INodeContent* GetContent();

	template<typename T, typename = typename std::enable_if<std::is_base_of<INodeContent, T>::value>::type>
	T* GetContent()
	{
		if (_content == nullptr)
			return nullptr;

		T* contentCasted = dynamic_cast<T*>(_content);
		return contentCasted;
	}

	///*template <typename T>
	//bool GetContent(T* outContent)
	//{
	//	if (_content == nullptr)
	//	{
	//		return false;
	//	}

	//	outContent = dynamic_cast<T*>(_content);
	//	return outContent != nullptr;
	//}*/

	void SetContent(INodeContent* newContent);
	void DrawContent(Vector2 offset);

	void Lock();
	void Unlock();

private:
	Vector2 _position;
	INodeContent* _content;
	std::mutex _classMutex;

};

