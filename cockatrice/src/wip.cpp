class DraggableItem : public QGraphicsItem
{
protected:
    QGraphicsItem *feedback;
    QGraphicsItem *childWhenDragging;

public:
    enum { Type = typeDraggable; };

    int type() const override
    {
        return Type;
    }

    DraggableItem(QGraphicsItem *child, QGraphicsItem *parent = nullptr);

    // TODO: onDragCancel?
    // NOTE: this is going to be CardItem (or AbstractCardItem?)
    // NOTE: never need to cast!
    // NOTE: DragTarget *toDragTarget(QGraphicsItem*)
    // (for compatible DragTarget)
};

DraggableItem::DraggableItem(QGraphicsItem *child, QGraphicsItem *parent = nullptr)
    : QGraphicsItem(parent), m_child(child)
{
    m_child->setParentItem(this);
}

class CardDragItem : public QGraphicsItem
{
protected:
    QPointF m_hotSpot; // Position of the hot spot relative to top left
    QPointF m_center; // Position of the item center relative to top left
    DragTarget *m_currentDragTarget; // Current drag target, or null
    CardItem *m_data; // Drag data, never null

public:
};

DragTarget *CardDragItem::findDragTarget(QPointF centerPos)
{
    QList<QGraphicsItem *> colliding
        {scene()->items(centerPos, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder,
                        static_cast<GameScene *>(scene())->getViewTransform())};

    for (auto *targetItem : colliding) {
        if (auto *dragTarget = qgraphicsitem_cast<DragTarget *>(targetItem))
            return dragTarget;
    }

    return nullptr;
}

void CardDragItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    m_data->paint(painter, option, widget);
}

QPointF CardDragItem::updateDragTarget(QPointF sceneTopLeftPos)
{
    QPointF sceneCenterPos = sceneTopLeftPos + m_center;

    DragTarget *dragTarget = findDragTarget(sceneCenterPos);

    // Update position to dragTarget coordinates
    QPointF targetCenterPos{};

    if (dragTarget)
        targetCenterPos = sceneCenterPos - dragTarget->scenePos();

    if (dragTarget != m_currentDragTarget) {
        if (m_currentDragTarget)
            m_currentDragTarget->dragLeave(m_data);

        if (dragTarget && dragTarget->dragEnter(m_data, targetCenterPos))
            m_currentDragTarget = dragTarget;
    } else if (dragTarget) { // Equal and not null
        dragTarget->dragMove(m_data, targetCenterPos);
    }

    return targetCenterPos;
}

void CardDragItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    setPos(event->scenePos() - m_hotSpot);

    updateDragTarget(event->scenePos() - m_hotSpot);
}

void CardDragItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);

    // Make sure we drop at the current mouse location
    QPointF centerPos = updateDragTarget(event->scenePos() - m_hotSpot);

    if (m_currentDragTarget) {
        m_currentDragTarget->dragAccept(m_data, centerPos);
    }

    scene()->removeItem(this);
    delete this; // We are done now.
}

class DragTargetItem : public QGraphicsItem
{
public:
    virtual bool dragEnter(CardItem *card, QPointF offset) = 0;
    virtual void dragMove(CardItem *card, QPointF offset);
    virtual void dragAccept(CardItem *card, QPointF offset) = 0;
    virtual void dragLeave(CardItem *card) = 0;
};

void DragTargetItem::dragMove(CardItem *card, QPointF offset)
{
    Q_UNUSED(card);
    Q_UNUSED(offset);
}
