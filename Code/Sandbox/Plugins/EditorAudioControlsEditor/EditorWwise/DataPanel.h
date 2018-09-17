// Copyright 2001-2018 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <QWidget>
#include <SharedData.h>

class QFilteringPanel;

namespace ACE
{
namespace Impl
{
namespace Wwise
{
class CFilterProxyModel;
class CItemModel;
class CTreeView;
class CImpl;

class CDataPanel final : public QWidget
{
public:

	CDataPanel() = delete;
	CDataPanel(CDataPanel const&) = delete;
	CDataPanel(CDataPanel&&) = delete;
	CDataPanel& operator=(CDataPanel const&) = delete;
	CDataPanel& operator=(CDataPanel&&) = delete;

	explicit CDataPanel(CImpl const& impl);
	virtual ~CDataPanel() override;

	void Reset();
	void OnAboutToReload();
	void OnReloaded();
	void OnConnectionAdded() const;
	void OnConnectionRemoved() const;
	void OnSelectConnectedItem(ControlId const id);

private:

	// QObject
	virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
	// ~QObject

	void OnContextMenu(QPoint const& pos);
	void PlayEvent();
	void StopEvent();
	void ClearFilters();

	CImpl const&             m_impl;
	CFilterProxyModel* const m_pFilterProxyModel;
	CItemModel* const        m_pModel;
	CTreeView* const         m_pTreeView;
	QFilteringPanel*         m_pFilteringPanel;
	int const                m_nameColumn;
};
} // namespace Wwise
} // namespace Impl
} // namespace ACE
