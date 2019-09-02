#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <list>

template<typename ValueType>
class Trie
{
public:
    Trie() { m_root = new Node(0); }
	~Trie() { delete m_root; }

	void reset()
	{
		delete m_root;
		this = new Node(0);
	}

	void insert(const std::string& key, const ValueType& value)
	{
		Node* curr = m_root;

		for (size_t i = 0; i < key.size(); i++)
		{
			char c = key[i];
			bool found = false;
			for (auto it = curr->m_children.begin(); it != curr->m_children.end(); it++)
			{
				Node* child = *it;
				if (c == child->m_label)
				{
					curr = child;
					found = true;
					break;
				}
			}
			if (!found)
			{
				Node* next = new Node(c);
				curr->m_children.push_back(next);
				curr = next;
			}
		}
		curr->m_values.push_back(value);
	}

	std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const
	{
		std::vector<ValueType> matches;
		char c = key[0];
		int mismatches = 0, pos = 1;
		Node* curr = m_root;
		for (auto it = curr->m_children.begin(); it != curr->m_children.end(); it++)
		{
			Node* child = *it;
			if (c == child->m_label)
			{
				child->findHelper(matches, key, exactMatchOnly, mismatches, pos);
				break;
			}
		}
		return matches;
	}

      // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

private:
	struct Node
	{
		Node(char label)
			: m_label(label)
		{}

		~Node()
		{
			for (auto it = m_children.begin(); it != m_children.end(); it++)
			{
				Node* curr = *it;
				delete curr;
			}
		}

		void findHelper(std::vector<ValueType>& matches, const std::string& key, bool exactMatchOnly, int mismatches, int pos)
		{
			if (mismatches > 1)
				return;

			if (key[pos] == 0)	//end of key path
			{
				for (auto it = m_values.begin(); it != m_values.end(); it++)
					matches.push_back(*it);

				return;
			}

			char c = key[pos];
			for (auto it = m_children.begin(); it != m_children.end(); it++)
			{
				Node* child = *it;
				if (c == child->m_label)
					child->findHelper(matches, key, exactMatchOnly, mismatches, pos + 1);
				else if (!exactMatchOnly)
					child->findHelper(matches, key, exactMatchOnly, mismatches + 1, pos + 1);
			}
		}

		char m_label;
		std::list<ValueType> m_values;
		std::vector<Node*> m_children;
	};

	Node* m_root;
};

#endif // TRIE_INCLUDED
