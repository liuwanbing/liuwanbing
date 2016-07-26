//
//  GGListClassCell.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGListClassCell.h"
#import "GGListClassModel.h"

@implementation GGListClassCell
- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}
-(void)setCellModel:(NSArray *)obj{
    UIScrollView *scrollView = [[UIScrollView alloc]initWithFrame:CGRectMake(0, 0, ScreenWidth, self.frame.size.height)];
    [self addSubview:scrollView];
    scrollView.showsHorizontalScrollIndicator = NO;
    scrollView.bounces = NO;
    for (NSInteger i=0; i<obj.count; i++) {
        UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
        btn.frame = CGRectMake(10+(ScreenWidth/5+20)*i, 0 , ScreenWidth/5, ScreenWidth/5);
        GGListClassModel *model = obj[i];
        UIImageView *imageV = [[UIImageView alloc]initWithFrame:CGRectMake(10 , 10 , ScreenWidth/7 , ScreenWidth/7)];
        imageV.layer.cornerRadius = imageV.frame.size.width/2;
        [imageV sd_setImageWithURL:[NSURL URLWithString:model.thnumb]];
        [btn addSubview:imageV];
        UILabel *label = [[UILabel alloc]initWithFrame:CGRectMake(0, ScreenWidth/7+15 , ScreenWidth/5, 20)];
        [btn addSubview:label];
        label.font = [UIFont systemFontOfSize:13];
        label.textAlignment = NSTextAlignmentCenter;
        label.text = model.title;
        [scrollView addSubview:btn];
    }
     scrollView.contentSize = CGSizeMake(ScreenWidth/4*obj.count+20, 0);
    

}
@end
