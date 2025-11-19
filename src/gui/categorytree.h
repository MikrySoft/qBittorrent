#pragma once

#include <QString>
#include <QAction>
#include <QMenu>
#include <vector>

class TreeNode
{
public:
    TreeNode(const QString& name, QAction* action = nullptr, QMenu* menu = nullptr, TreeNode* parent = nullptr);
    ~TreeNode();

    // Getters and setters
    QString getName() const;
    QString getFullPath() const;
    QAction* getAction() const;
    QMenu* getMenu() const;
    TreeNode* getParent() const;

    void setName(const QString& name);
    void setAction(QAction* action);
    void setMenu(QMenu* menu);
    void setParent(TreeNode* parent);

    // Methods for managing child nodes
    void addChild(TreeNode* child);
    std::vector<TreeNode*> getChildren() const;
    bool hasChildren() const;

    void traverseDFS(const std::function<void(TreeNode*)>& action);
    TreeNode* addNodeByPath(const QString& path);

private:
    QString m_name;
    QString m_fullPath;
    QAction* m_action;
    QMenu* m_menu;
    TreeNode* m_parent;
    std::vector<TreeNode*> m_children;
    QString constructFullPath() const;
};
