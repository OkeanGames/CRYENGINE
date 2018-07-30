// Copyright 2001-2018 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include "EditorCommonAPI.h"

#include <QWidget>

#include "EditorFramework/Editor.h"
#include "ProxyModels/ItemModelAttribute.h"

class CAsset;
class CAssetDropHandler;
class CAssetFolderFilterModel;
class CAssetFoldersView;
class CAssetType;
class CBreadcrumbsBar;
class CLineEditDelegate;
class QAdvancedTreeView;
class QAttributeFilterProxyModel;
class QFilteringPanel;
class QItemSelectionModel;
class QMenu;
class QSplitter;
class QThumbnailsView;
class QTimer;
class QToolButton;

//Thumbnails should now be used, leaving for now in case it is temporarily useful
#define ASSET_BROWSER_USE_PREVIEW_WIDGET 0

//! The dockable class for the Asset Browser
class EDITOR_COMMON_API CAssetBrowser : public CDockableEditor
{
	Q_OBJECT

public:
	static CCrySignal<void(CAbstractMenu&, const std::shared_ptr<IUIContext>&)> s_signalMenuCreated;
	static CCrySignal<void(CAbstractMenu&, const std::vector<CAsset*>&, const std::vector<string>& folders, const std::shared_ptr<IUIContext>&) > s_signalContextMenuRequested;

	CAssetBrowser(bool bHideEngineFolder = false, QWidget* pParent = nullptr);
	virtual ~CAssetBrowser();

	std::vector<CAsset*> GetSelectedAssets() const;
	CAsset*              GetLastSelectedAsset() const;

	void                 SelectAsset(const char* szPath) const;
	void                 SelectAsset(const CAsset& asset) const;

	QStringList          GetSelectedFolders() const;

	//CEditor implementation
	virtual const char* GetEditorName() const override { return "Asset Browser"; }
	virtual void        SetLayout(const QVariantMap& state) override;
	virtual QVariantMap GetLayout() const override;

	void                GrabFocusSearchBar() { OnFind(); }
	QFilteringPanel*    GetFilterPanel();

	enum ViewMode
	{
		Details,
		Thumbnails,
		VSplit,
		HSplit,

		Max
	};

	void    SetViewMode(ViewMode viewMode);
	void    SetRecursiveView(bool recursiveView);
	void    SetRecursiveSearch(bool recursiveSearch);

	CAsset* QueryNewAsset(const CAssetType& type, const void* pTypeSpecificParameter);

	void NotifyContextMenuCreation(CAbstractMenu& menu, const std::vector<CAsset*>& assets, const std::vector<string>& folders);

signals:
	//! This signal is emitted whenever the selection of folders or assets changes.
	void SelectionChanged();

protected:
	// Drag and drop.
	virtual void dragEnterEvent(QDragEnterEvent* pEvent) override;
	virtual void dropEvent(QDropEvent* pEvent) override;
	virtual void dragMoveEvent(QDragMoveEvent* pEvent) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* pEvent) override;

	virtual void mouseReleaseEvent(QMouseEvent* pEvent) override;

	//! Returns whether there is a folder under the mouse cursor. If there is, \p folder is assigned its path.
	bool GetDropFolder(string& folder) const;

	//! Sets \p folder to where assets should be imported. Returns whether the import location is unambiguous.
	bool                        GetImportFolder(string& folder) const;

	QAttributeFilterProxyModel* GetAttributeFilterProxyModel();
	QItemSelectionModel*        GetItemSelectionModel();
	QAdvancedTreeView*          GetDetailsView();
	QThumbnailsView*            GetThumbnailsView();

	// The widget has to be visible.
	void         ScrollToSelected();

	bool         ValidatePath(const QString);

	virtual void OnActivated(CAsset* pAsset);
	virtual void OnActivated(const QString& folder);

	virtual void UpdatePreview(const QModelIndex& currentIndex);
private:

	void               InitNewNameDelegates();
	void               InitViews(bool bHideEngineFolder);
	void               InitMenus();

	void               InitAssetsView();
	void               InitDetailsView();
	void               InitThumbnailsView();
	void               AddViewModeButton(ViewMode viewMode, const char* szIconPath, const char* szToolTip, QMenu* pMenu = nullptr);
	QWidget*           CreateAssetsViewSelector();

	void               FillCreateAssetMenu(CAbstractMenu* menu, const QString& folder);

	void               BeginCreateAsset(const CAssetType& type, const void* pTypeSpecificParameter);
	void               EndCreateAsset();

	QAbstractItemView* GetFocusedView() const;

	bool               eventFilter(QObject* object, QEvent* event) override;

	//extract actual content from the selection for further processing
	void ProcessSelection(std::vector<CAsset*>& assets, std::vector<string>& folders) const;

	void OnContextMenu();
	void AppendFilterDependenciesActions(CAbstractMenu* pAbstractMenu, const CAsset* pAsset);
	void OnFolderViewContextMenu();

	void CreateContextMenu(bool isFolderView = false);

	std::vector<string> GetSelectedFoldersInFolderView();

	void BuildContextMenuForEmptiness(CAbstractMenu& abstractMenu);
	void BuildContextMenuForFolders(const std::vector<string>& folders, CAbstractMenu &abstractMenu);
	void BuildContextMenuForAssets(const std::vector<CAsset*>& assets, const std::vector<string>& folders, CAbstractMenu &abstractMenu);

	void OnFolderSelectionChanged(const QStringList& selectedFolders);
	void OnActivated(const QModelIndex& index);
	void OnCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
	void OnImport();
	void OnReimport(const std::vector<CAsset*>& assets);
	void OnDelete(const std::vector<CAsset*>& assets);
	void OnMove(const std::vector<CAsset*>& assets, const QString& destinationFolder);
	void OnRenameAsset(CAsset& asset);
	void OnRenameFolder(const QString& folder);
	void OnCreateFolder(const QString& parentFolder);
	void OnOpenInExplorer(const QString& folder);
	void OnNavBack();
	void OnNavForward();
	void OnBreadcrumbClick(const QString& text, const QVariant& data);
	void OnBreadcrumbsTextChanged(const QString& text);

	void GenerateThumbnailsAsync(const string& folder, const std::function<void()>& finalize = std::function<void()>());

	void UpdateModels();
	void UpdateNavigation(bool clearHistory);
	void UpdateBreadcrumbsBar(const QString& path);

	void EditNewAsset();

	//////////////////////////////////////////////////////////////////////////
	// CEditor impl
	bool OnFind() override;
	bool OnDelete() override;
	bool OnOpen() override;
	//////////////////////////////////////////////////////////////////////////

	//ui components
	QAdvancedTreeView*                          m_detailsView;
	QThumbnailsView*                            m_thumbnailView;
	std::unique_ptr<CLineEditDelegate>          m_detailsViewNewNameDelegate; // Note that delegates are not owned by view.
	std::unique_ptr<CLineEditDelegate>          m_thumbnailViewNewNameDelegate;
	QItemSelectionModel*                        m_selection;
	QSplitter*                                  m_mainViewSplitter;
	QFilteringPanel*                            m_filterPanel;
	QSplitter*                                  m_foldersSplitter;
	CAssetFoldersView*                          m_foldersView;
	std::unique_ptr<QAttributeFilterProxyModel> m_pAttributeFilterProxyModel;
	std::unique_ptr<CAssetFolderFilterModel>    m_pFolderFilterModel;
	std::unique_ptr<CAssetDropHandler>          m_pAssetDropHandler;
	QToolButton*                                m_backButton;
	QToolButton*                                m_forwardButton;
	CBreadcrumbsBar*                            m_breadcrumbs;
	QLabel* m_multipleFoldersLabel;
	QButtonGroup*                               m_viewModeButtons;
	std::unique_ptr<CAbstractMenu>              m_thumbnailSizeMenu;

	//state variables
	ViewMode             m_viewMode;
	bool                 m_recursiveView;
	bool                 m_recursiveSearch;
	QVector<QStringList> m_navigationHistory;
	int                  m_navigationIndex;    //-1 is "all assets"
	bool                 m_dontPushNavHistory; //true when folder changes are triggered by back/forward buttons

#if ASSET_BROWSER_USE_PREVIEW_WIDGET
	QContainer*             m_previewWidget;
#endif
	std::unique_ptr<QTimer> m_pQuickEditTimer;
};
