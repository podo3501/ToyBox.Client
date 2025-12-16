#pragma once

template<typename CommandType>
class CommandHistory
{
public:
	~CommandHistory() = default;
	CommandHistory() = default;

	bool Undo() noexcept;
	bool Redo() noexcept;

protected:
	template <typename CommandTypeDerived, typename... ParamTypes>
	bool ApplyCommand(ParamTypes&&... params);
	void AddOrMergeCommand(unique_ptr<CommandType> command) noexcept;

private:
	unique_ptr<CommandType> TryMergeCommand(unique_ptr<CommandType> cmd) noexcept;

	vector<unique_ptr<CommandType>> m_commandList;
	int m_index{ -1 };
};

template<typename CommandType>
bool CommandHistory<CommandType>::Undo() noexcept
{
	if (m_index < 0) return true;

	auto& cmd = m_commandList[m_index];
	auto result = cmd->Undo();

	if (!result)
	{
		assert(false && "Undo함수는 실행하는 것을 돌리는 것이기 때문에 실패해서는 안된다.");
		return result;
	}

	m_index--;
	return true;
}

template<typename CommandType>
bool CommandHistory<CommandType>::Redo() noexcept
{
	if (m_index + 1 >= static_cast<int>(m_commandList.size())) return true;

	auto& cmd = m_commandList[m_index + 1];
	auto result = cmd->Redo();

	if (!result)
	{
		assert(false && "Redo함수는 실행했던 것을 다시 실행하는 것이기 때문에 실패해서는 안된다.");
		return result;
	}

	m_index++;
	return true;
}

template<typename CommandType>
unique_ptr<CommandType> CommandHistory<CommandType>::TryMergeCommand(unique_ptr<CommandType> cmd) noexcept
{
	if (m_commandList.empty()) return cmd;

	auto& preCmd = m_commandList.back();
	return preCmd->Merge(move(cmd));
}

template<typename CommandType>
void CommandHistory<CommandType>::AddOrMergeCommand(unique_ptr<CommandType> command) noexcept
{
	if (m_index < static_cast<int>(m_commandList.size()) - 1)
		m_commandList.resize(m_index + 1);

	//Merge를 시도하고 실패하면 command는 다시 돌려준다. 
	// 아니면 IsMerge를 불러줘야 하는데 최대한 public 함수를 안 내놓는게 좋은 코딩법이다.
	command = TryMergeCommand(move(command));
	if (!command) return;

	m_commandList.emplace_back(move(command));
	m_index++;
}

template<typename CommandType>
template <typename CommandTypeDerived, typename... ParamTypes>
bool CommandHistory<CommandType>::ApplyCommand(ParamTypes&&... params)
{
	unique_ptr<CommandType> command = make_unique<CommandTypeDerived>(forward<ParamTypes>(params)...);
	if (!command->Execute()) return false;

	AddOrMergeCommand(move(command));
	return true;
}