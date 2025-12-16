#pragma once

class ToolLoop;
class FileTab;

class MenuBar
{
public:
	MenuBar() = delete;
	MenuBar(ToolLoop* toolLoop);
	~MenuBar();

	void Update() const;
	bool Render() const;

private:
	unique_ptr<FileTab> m_fileTab;
};
