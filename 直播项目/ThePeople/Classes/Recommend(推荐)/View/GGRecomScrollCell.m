//
//  GGRecomScrollCell.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGRecomScrollCell.h"
#import "CycleScrollView.h"
#import "GGRecomScrModel.h"
@interface GGRecomScrollCell()
@property (nonatomic , retain)CycleScrollView *mainScorllView;
@end

@implementation GGRecomScrollCell
- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}
-(void)setCellModel:(NSArray *)obj{
    if (obj) {
    NSMutableArray *viewsArray = [@[] mutableCopy];
    for (int i = 0; i<obj.count; i++) {
        UIImageView *imgView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, ScreenWidth,self.frame.size.height )];
        UILabel *label = [[UILabel alloc]initWithFrame:CGRectMake(0, self.frame.size.height-20, ScreenWidth, 20)];
        [imgView addSubview:label];
        GGRecomScrModel *model = [[GGRecomScrModel alloc]init];
        model = obj[i];
        label.text = [NSString stringWithFormat:@"     %@",model.title];
        label.textColor = [UIColor whiteColor];
        label.backgroundColor = [UIColor colorWithRed:0/255.0 green:0/255.0 blue:0/255.0 alpha:0.5];
        [imgView sd_setImageWithURL:[NSURL URLWithString:model.thumb]];
        [viewsArray addObject:imgView];
    }
    self.mainScorllView = [[CycleScrollView alloc]initWithFrame:CGRectMake(0, 0, ScreenWidth, self.frame.size.height) animationDuration:2.0 Count:viewsArray.count];
    self.mainScorllView.fetchContentViewAtIndex = ^UIView *(NSInteger pageIndex){
        return viewsArray[pageIndex];
    };
    
    self.mainScorllView.totalPagesCount = ^NSInteger(void){
        return viewsArray.count;
    };
    
    self.mainScorllView.TapActionBlock = ^(NSInteger pageIndex){
    
        //            RoomDetailController *rvc = [[RoomDetailController alloc]initWithVideoId:((AdModel *) [array objectAtIndex:pageIndex]).room.videoId];
        //            [self.viewController.navigationController pushViewController:rvc animated:YES];
    
        //            NSLog(@"点击了第%ld个",pageIndex);
    };
    [self addSubview:self.mainScorllView];
    }
}
@end
