/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "projectexplorer_export.h"

#include <utils/fileutils.h>

#include <QFutureInterface>
#include <QIcon>

#include <QObject>
#include <QStringList>
#include <QDebug>

#include <functional>

QT_BEGIN_NAMESPACE
class QFileInfo;
QT_END_NAMESPACE

namespace ProjectExplorer {
class RunConfiguration;

enum class NodeType : quint16 {
    File = 1,
    Folder,
    VirtualFolder,
    Project,
    Session
};

// File types common for qt projects
enum class FileType : quint16 {
    Unknown = 0,
    Header,
    Source,
    Form,
    StateChart,
    Resource,
    QML,
    Project,
    FileTypeSize
};

enum ProjectAction {
    // Special value to indicate that the actions are handled by the parent
    InheritedFromParent,
    AddSubProject,
    RemoveSubProject,
    // Let's the user select to which project file
    // the file is added
    AddNewFile,
    AddExistingFile,
    // Add files, which match user defined filters,
    // from an existing directory and its subdirectories
    AddExistingDirectory,
    // Removes a file from the project, optionally also
    // delete it on disc
    RemoveFile,
    // Deletes a file from the file system, informs the project
    // that a file was deleted
    // DeleteFile is a define on windows...
    EraseFile,
    Rename,
    DuplicateFile,
    // hides actions that use the path(): Open containing folder, open terminal here and Find in Directory
    HidePathActions,
    HideFileActions,
    HideFolderActions,
    HasSubProjectRunConfigurations
};

class Node;
class FileNode;
class FileContainerNode;
class FolderNode;
class ProjectNode;
class SessionNode;
class NodesVisitor;
class SessionManager;

// Documentation inside.
class PROJECTEXPLORER_EXPORT Node
{
public:
    enum PriorityLevel {
        DefaultPriority = 0,
        DefaultFilePriority = 100000,
        DefaultFolderPriority = 200000,
        DefaultVirtualFolderPriority = 300000,
        DefaultProjectPriority = 400000,
        DefaultProjectFilePriority = 500000
    };

    virtual ~Node() = default;
    NodeType nodeType() const;
    int priority() const;

    ProjectNode *parentProjectNode() const; // parent project, will be nullptr for the top-level project
    FolderNode *parentFolderNode() const; // parent folder or project

    ProjectNode *managingProject();  // project managing this node.
                                     // result is nullptr if node is the SessionNode
                                     // or node if node is a ProjectNode directly below SessionNode
                                     // or node->parentProjectNode() for all other cases.
    const ProjectNode *managingProject() const; // see above.

    const Utils::FileName &filePath() const;  // file system path
    int line() const;
    virtual QString displayName() const;
    virtual QString tooltip() const;
    bool isEnabled() const;

    virtual QList<ProjectAction> supportedActions(Node *node) const;

    void setEnabled(bool enabled);
    void setAbsoluteFilePathAndLine(const Utils::FileName &filePath, int line);

    void emitNodeUpdated();

    virtual Node *trim(const QSet<Node *> &keepers);

    virtual FileNode *asFileNode() { return nullptr; }
    virtual const FileNode *asFileNode() const { return nullptr; }
    virtual FolderNode *asFolderNode() { return nullptr; }
    virtual const FolderNode *asFolderNode() const { return nullptr; }
    virtual ProjectNode *asProjectNode() { return nullptr; }
    virtual const ProjectNode *asProjectNode() const { return nullptr; }
    virtual SessionNode *asSessionNode() { return nullptr; }
    virtual const SessionNode *asSessionNode() const { return nullptr; }

    static bool sortByPath(Node *a, Node *b);

protected:
    Node(NodeType nodeType, const Utils::FileName &filePath, int line = -1);

    void setPriority(int priority);
    void setParentFolderNode(FolderNode *parentFolder);

    void emitNodeSortKeyAboutToChange();
    void emitNodeSortKeyChanged();

private:
    FolderNode *m_parentFolderNode = nullptr;
    Utils::FileName m_filePath;
    int m_line = -1;
    int m_priority = DefaultPriority;
    const NodeType m_nodeType;
    bool m_isEnabled = true;
};

class PROJECTEXPLORER_EXPORT FileNode : public Node
{
public:
    FileNode(const Utils::FileName &filePath, const FileType fileType, bool generated, int line = -1);

    FileType fileType() const;
    bool isGenerated() const;

    FileNode *asFileNode() final { return this; }
    const FileNode *asFileNode() const final { return this; }

    static QList<FileNode *> scanForFiles(const Utils::FileName &directory,
                                          const std::function<FileNode *(const Utils::FileName &fileName)> factory,
                                          QFutureInterface<QList<FileNode *>> *future = nullptr);

private:
    // managed by ProjectNode
    friend class FolderNode;
    friend class ProjectNode;

    FileType m_fileType;
    bool m_generated;
};

// Documentation inside.
class PROJECTEXPLORER_EXPORT FolderNode : public Node
{
public:
    explicit FolderNode(const Utils::FileName &folderPath, NodeType nodeType = NodeType::Folder,
                        const QString &displayName = QString());
    ~FolderNode() override;

    QString displayName() const override;
    QIcon icon() const;

    Node *trim(const QSet<Node *> &keepers) override;

    QList<FileNode *> fileNodes() const;
    FileNode *fileNode(const Utils::FileName &file) const;
    FileNode *recursiveFileNode(const Utils::FileName &file) const;
    QList<FileNode *> recursiveFileNodes() const;
    QList<FolderNode *> folderNodes() const;
    FolderNode *folderNode(const Utils::FileName &directory) const;
    FolderNode *recursiveFindOrCreateFolderNode(const QString &directory,
                                                const Utils::FileName &overrideBaseDir = Utils::FileName());
    void buildTree(QList<FileNode *> &files, const Utils::FileName &overrideBaseDir = Utils::FileName());

    virtual void accept(NodesVisitor *visitor);

    void setDisplayName(const QString &name);
    void setIcon(const QIcon &icon);

    virtual QString addFileFilter() const;

    virtual bool addFiles(const QStringList &filePaths, QStringList *notAdded = 0);
    virtual bool removeFiles(const QStringList &filePaths, QStringList *notRemoved = 0);
    virtual bool deleteFiles(const QStringList &filePaths);
    virtual bool canRenameFile(const QString &filePath, const QString &newFilePath);
    virtual bool renameFile(const QString &filePath, const QString &newFilePath);

    class AddNewInformation
    {
    public:
        AddNewInformation(const QString &name, int p)
            :displayName(name), priority(p)
        { }
        QString displayName;
        int priority;
    };

    virtual AddNewInformation addNewInformation(const QStringList &files, Node *context) const;


    // determines if node will be shown in the flat view, by default folder and projects aren't shown
    virtual bool showInSimpleTree() const;

    void addFileNodes(const QList<FileNode*> &files);
    void removeFileNodes(const QList<FileNode*> &files);

    void addFolderNodes(const QList<FolderNode*> &subFolders);
    void removeFolderNodes(const QList<FolderNode*> &subFolders);

    FolderNode *asFolderNode() final { return this; }
    const FolderNode *asFolderNode() const final { return this; }

protected:
    QList<FolderNode*> m_folderNodes;
    QList<FileNode*> m_fileNodes;

private:
    QString m_displayName;
    mutable QIcon m_icon;

    // managed by ProjectNode
    friend class ProjectNode;
};

class PROJECTEXPLORER_EXPORT VirtualFolderNode : public FolderNode
{
public:
    explicit VirtualFolderNode(const Utils::FileName &folderPath, int priority);
};

// Documentation inside.
class PROJECTEXPLORER_EXPORT ProjectNode : public FolderNode
{
public:
    QString vcsTopic() const;

    virtual bool canAddSubProject(const QString &proFilePath) const;
    virtual bool addSubProjects(const QStringList &proFilePaths);
    virtual bool removeSubProjects(const QStringList &proFilePaths);

    bool addFiles(const QStringList &filePaths, QStringList *notAdded = 0) override;
    bool removeFiles(const QStringList &filePaths, QStringList *notRemoved = 0) override;
    bool deleteFiles(const QStringList &filePaths) override;
    bool canRenameFile(const QString &filePath, const QString &newFilePath) override;
    bool renameFile(const QString &filePath, const QString &newFilePath) override;

    // by default returns false
    virtual bool deploysFolder(const QString &folder) const;

    virtual QList<RunConfiguration *> runConfigurations() const;

    void accept(NodesVisitor *visitor) override;

    ProjectNode *projectNode(const Utils::FileName &file) const;
    // all subFolders that are projects
    QList<ProjectNode*> projectNodes() const;
    void addProjectNodes(const QList<ProjectNode*> &subProjects);
    void removeProjectNodes(const QList<ProjectNode*> &subProjects);

    ProjectNode *asProjectNode() final { return this; }
    const ProjectNode *asProjectNode() const final { return this; }

    Node *trim(const QSet<Node *> &keepers) override;

protected:
    // this is just the in-memory representation, a subclass
    // will add the persistent stuff
    explicit ProjectNode(const Utils::FileName &projectFilePath);

private:
    QList<ProjectNode*> m_projectNodes;

    // let SessionNode call setParentFolderNode
    friend class SessionNode;
};

// Documentation inside.
class PROJECTEXPLORER_EXPORT SessionNode : public FolderNode
{
    friend class SessionManager;
public:
    SessionNode();

    QList<ProjectAction> supportedActions(Node *node) const override;

    QList<ProjectNode*> projectNodes() const;

    QString addFileFilter() const override;

    void accept(NodesVisitor *visitor) override;

    bool showInSimpleTree() const override;
    void projectDisplayNameChanged(Node *node);

    SessionNode *asSessionNode() final { return this; }
    const SessionNode *asSessionNode() const final { return this; }

protected:
    void addProjectNodes(const QList<ProjectNode*> &projectNodes);
    void removeProjectNodes(const QList<ProjectNode*> &projectNodes);

private:
    QList<ProjectNode*> m_projectNodes;
};

template<class T1, class T3>
bool isSorted(const T1 &list, T3 sorter)
{
    typename T1::const_iterator it, iit, end;
    end = list.constEnd();
    it = list.constBegin();
    if (it == end)
        return true;

    iit = list.constBegin();
    ++iit;

    while (iit != end) {
        if (!sorter(*it, *iit))
            return false;
        it = iit++;
    }
    return true;
}

template <class T1, class T2, class T3>
void compareSortedLists(T1 oldList, T2 newList, T1 &removedList, T2 &addedList, T3 sorter)
{
    Q_ASSERT(isSorted(oldList, sorter));
    Q_ASSERT(isSorted(newList, sorter));

    typename T1::const_iterator oldIt, oldEnd;
    typename T2::const_iterator newIt, newEnd;

    oldIt = oldList.constBegin();
    oldEnd = oldList.constEnd();

    newIt = newList.constBegin();
    newEnd = newList.constEnd();

    while (oldIt != oldEnd && newIt != newEnd) {
        if (sorter(*oldIt, *newIt)) {
            removedList.append(*oldIt);
            ++oldIt;
        } else if (sorter(*newIt, *oldIt)) {
            addedList.append(*newIt);
            ++newIt;
        } else {
            ++oldIt;
            ++newIt;
        }
    }

    while (oldIt != oldEnd) {
        removedList.append(*oldIt);
        ++oldIt;
    }

    while (newIt != newEnd) {
        addedList.append(*newIt);
        ++newIt;
    }
}

template <class T1, class T3>
T1 subtractSortedList(T1 list1, T1 list2, T3 sorter)
{
    Q_ASSERT(isSorted(list1, sorter));
    Q_ASSERT(isSorted(list2, sorter));

    typename T1::const_iterator list1It, list1End;
    typename T1::const_iterator list2It, list2End;

    list1It = list1.constBegin();
    list1End = list1.constEnd();

    list2It = list2.constBegin();
    list2End = list2.constEnd();

    T1 result;

    while (list1It != list1End && list2It != list2End) {
        if (sorter(*list1It, *list2It)) {
            result.append(*list1It);
            ++list1It;
        } else if (sorter(*list2It, *list1It)) {
            qWarning() << "subtractSortedList: subtracting value that isn't in set";
        } else {
            ++list1It;
            ++list2It;
        }
    }

    while (list1It != list1End) {
        result.append(*list1It);
        ++list1It;
    }

    return result;
}

} // namespace ProjectExplorer

Q_DECLARE_METATYPE(ProjectExplorer::Node *)
Q_DECLARE_METATYPE(ProjectExplorer::FolderNode *)
