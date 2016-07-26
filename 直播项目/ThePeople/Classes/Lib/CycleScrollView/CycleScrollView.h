//NSMutableArray *viewsArray = [@[] mutableCopy];
//for (int i = 0; i<_array.count; i++) {
//    UIImageView *imgView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, screenW,self.frame.size.height )];
//    [imgView sd_setImageWithURL:[NSURL URLWithString:_array[i][@"theme_image"]]];
//    [viewsArray addObject:imgView];
//}
//self.mainScorllView = [[CycleScrollView alloc]initWithFrame:CGRectMake(0, 0, screenW, self.frame.size.height) animationDuration:2.0 Count:viewsArray.count];
//self.mainScorllView.fetchContentViewAtIndex = ^UIView *(NSInteger pageIndex){
//    return viewsArray[pageIndex];
//};
//
//self.mainScorllView.totalPagesCount = ^NSInteger(void){
//    return viewsArray.count;
//};
//
//self.mainScorllView.TapActionBlock = ^(NSInteger pageIndex){
//    
//    //            RoomDetailController *rvc = [[RoomDetailController alloc]initWithVideoId:((AdModel *) [array objectAtIndex:pageIndex]).room.videoId];
//    //            [self.viewController.navigationController pushViewController:rvc animated:YES];
//    
//    //            NSLog(@"点击了第%ld个",pageIndex);
//};
//[self addSubview:self.mainScorllView];
//}

#import <UIKit/UIKit.h>

@interface CycleScrollView : UIView

@property (nonatomic , readonly) UIScrollView *scrollView;
/**
 *  初始化
 *
 *  @param frame             frame
 *  @param animationDuration 自动滚动的间隔时长。如果<=0，不自动滚动。
 *
 *  @return instance
 */
- (id)initWithFrame:(CGRect)frame animationDuration:(NSTimeInterval)animationDuration Count:(NSInteger)count;

/**
 数据源：获取总的page个数
 **/
@property (nonatomic , copy) NSInteger (^totalPagesCount)(void);
/**
 数据源：获取第pageIndex个位置的contentView
 **/
@property (nonatomic , copy) UIView *(^fetchContentViewAtIndex)(NSInteger pageIndex);
/**
 当点击的时候，执行的block
 **/
@property (nonatomic , copy) void (^TapActionBlock)(NSInteger pageIndex);

@end