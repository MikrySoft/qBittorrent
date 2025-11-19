#include "categorytree.h"

#include <QDebug>
#include <QMenu>
#include <QRegularExpression>
#include <QSet>
#include <queue>

#include "base/global.h"
#include "base/logger.h"
#include "base/path.h"
#include "base/utils/compare.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "utils.h"
#include <stack>

// Constructor
TreeNode::TreeNode(const QString& name, QAction* action, QMenu* menu, TreeNode* parent)
    : m_name(name)
    , m_action(action)
    , m_menu(menu)
    , m_parent(parent)
{

    // Set m_fullPath based on the parent's m_fullPath and current node's name
    m_fullPath = constructFullPath();

    // Add this node to the parent's children if parent is not null
    if (parent)
        parent->addChild(this);
}

// Destructor
TreeNode::~TreeNode()
{
    for (TreeNode* child : m_children)
        delete child;
    m_children.clear();
    // Clean up dynamically allocated QAction and QMenu if necessary
    // Be cautious to avoid double-free if QAction or QMenu is shared.
}

// Getters
QString TreeNode::getName() const
{
    return m_name;
}

QString TreeNode::getFullPath() const
{
    return m_fullPath;
}

QAction* TreeNode::getAction() const
{
    return m_action;
}

QMenu* TreeNode::getMenu() const
{
    return m_menu;
}

TreeNode* TreeNode::getParent() const
{
    return m_parent;
}

// Setters
void TreeNode::setName(const QString& name)
{
    m_name = name;
    // Update the full path whenever the name changes
    m_fullPath = constructFullPath();
}

void TreeNode::setAction(QAction* action)
{
    m_action = action;
}

void TreeNode::setMenu(QMenu* menu)
{
    m_menu = menu;
}

void TreeNode::setParent(TreeNode* parent)
{
    m_parent = parent;
    // Update the full path whenever the parent changes
    m_fullPath = constructFullPath();
}

// Managing child nodes
void TreeNode::addChild(TreeNode* child)
{
    m_children.push_back(child);
}

std::vector<TreeNode*> TreeNode::getChildren() const
{
    return m_children;
}

QString TreeNode::constructFullPath() const
{
    // If there is a parent, concatenate its path with this node's name
    if (m_parent) {
        QString path;
        path = u"%1/%2"_s;
        path = path.arg(m_parent->getFullPath()).arg(m_name);
        return path;
    }
    // If no parent, this is the root node
    return m_name;
}

// DFS traversal
void TreeNode::traverseDFS(const std::function<void(TreeNode*)>& action)
{
    std::stack<TreeNode*> nodeStack;
    nodeStack.push(this);

    while (!nodeStack.empty()) {
        TreeNode* currentNode = nodeStack.top();
        nodeStack.pop();

        // Perform the provided action on the current node
        action(currentNode);

        // Push all children onto the stack (in reverse order to maintain DFS order)
        for (auto it = currentNode->m_children.rbegin(); it != currentNode->m_children.rend(); ++it)
            if (*it)
                nodeStack.push(*it);
    }
}

bool TreeNode::hasChildren() const
{
    return !m_children.empty();
}

TreeNode* TreeNode::addNodeByPath(const QString& path)
{
    QStringList nodeNames = path.split(u"/"_s);  // Split the path by '/'
    TreeNode* currentNode = this;  // Start from the current node
    // Traverse through the tree or create nodes if they don't exist
    for (const QString& nodeName : nodeNames) {
        bool found = false;

        // Check if the current node already has a child with the given name
        for (TreeNode* child : currentNode->m_children) {
            if (child->getName() == nodeName) {
                currentNode = child;  // Move to the existing child node
                found = true;
                break;
            }
        }
        // If no existing node was found, create a new one
        if (!found) {
            TreeNode* newNode = new TreeNode(nodeName, nullptr, nullptr, currentNode);  // Create new node
            currentNode = newNode;  // Move to the new node
        }
    }
    return currentNode;  // Return the last node created or found
}
